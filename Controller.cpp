#include "Instruction.hpp"
#include "fstream"
#include <sstream>
#include <map>
#include <iostream>
#include <queue>
#include <stdexcept>
@@ -249,3 +250,208 @@ void Controller::addInstruction(Instruction i)
}



// ─────────────────────────────────────────────────────────────────────────────
// writeToCache
//
// Writes `value` into the cache line that holds `addr` and marks the line
// dirty.  Does NOT touch main memory — that is deferred until eviction.
//
// Parameters:
//   addr             – full ADDRESS_SIZE-bit binary address string
//   value            – binary string to write; must be 32 chars for a word
//                      write or 8 chars for a byte write
//   wordGranularity  – true  : overwrite the entire word selected by the
//                               word-offset bits of `addr`
//                      false : overwrite only the single byte selected by
//                               both the word-offset and byte-offset bits
//
// Returns true on success.
// Returns false if the addressed block is not currently resident in cache
// (call fetchBlock(addr) first, then retry).
// ─────────────────────────────────────────────────────────────────────────────
bool Controller::writeToCache(const string& addr, const string& value, bool wordGranularity)
{
    // The addressed line must already be in cache.
    if (!hit(addr))
    {
        cout << "writeToCache: cache miss at " << addr
             << " — fetch the block first\n";
        return false;
    }

    int lineIdx = getBlockIdx(addr);
    int wordIdx = getWordOffset(addr);

    CacheLine* line = (*cache)[lineIdx];

    if (wordGranularity)
    {
        // ── Word write ────────────────────────────────────────────────────
        if (value.size() != 32)
        {
            cout << "writeToCache: word value must be 32 bits, got "
                 << value.size() << "\n";
            return false;
        }
        (*line)[wordIdx] = value;
        cout << "writeToCache: wrote word to line " << lineIdx
             << " word " << wordIdx << "\n";
    }
    else
    {
        // ── Byte write ────────────────────────────────────────────────────
        if (value.size() != 8)
        {
            cout << "writeToCache: byte value must be 8 bits, got "
                 << value.size() << "\n";
            return false;
        }

        int byteOffset = getByteOffset(addr);    // 0–3 within the word
        string word    = line->getWord(wordIdx); // current 32-bit word

        // Splice the new byte into the correct position.
        word.replace(8 * byteOffset, 8, value);
        (*line)[wordIdx] = word;

        cout << "writeToCache: wrote byte to line " << lineIdx
             << " word " << wordIdx
             << " byte " << byteOffset << "\n";
    }

    // Mark the line dirty so eviction knows it needs a write-back.
    line->setDirty();
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// writeBackLine  (private helper)
//
// Writes every word of cache line `lineIdx` back to the corresponding byte
// rows in ram.txt, then invalidates the line.
//
// ram.txt row format (one row per byte):  "xAAAA BBBBBBBB\n"
// where AAAA is the 4-digit hex address and BBBBBBBB is the 8-bit value.
//
// Strategy: stream through the whole file, replacing rows whose addresses
// fall inside this cache line's range, then atomically rename.
// ─────────────────────────────────────────────────────────────────────────────
bool Controller::writeBackLine(int lineIdx)
{
    CacheLine* line = (*cache)[lineIdx];

    int nLines   = cache->getNLines();
    int nWords   = cache->getNWords();
    int idxBits  = (int)log2(nLines);
    int wordBits = (int)log2(nWords);
    int byteBits = 2; // always 2 (byte offset within a 4-byte word)

    // Build the full binary address of byte 0 of this cache line:
    //   base = tag ++ lineIndex (idxBits wide) ++ all-zero offsets
    string tag = line->getTag();

    string idxStr = "";
    for (int b = idxBits - 1; b >= 0; --b)
        idxStr += ((lineIdx >> b) & 1) ? '1' : '0';

    string zeroOffset(wordBits + byteBits, '0');
    string baseAddrBin = tag + idxStr + zeroOffset;
    long long int baseAddr = (long long int)binToInt(baseAddrBin);

    // Build a patch map: byte_address → 8-bit binary string
    // Decompose each 32-bit word into 4 big-endian bytes.
    map<long long int, string> patchMap;
    for (int w = 0; w < nWords; ++w)
    {
        string word = line->getWord(w); // 32-bit binary string
        long long int wordBase = baseAddr + (long long int)w * 4;
        for (int b = 0; b < 4; ++b)
            patchMap[wordBase + b] = word.substr(8 * b, 8);
    }

    const string ramPath = "ram.txt";
    const string tmpPath = "ram_tmp.txt";

    ifstream fin(ramPath);
    ofstream fout(tmpPath);

    if (!fin.is_open() || !fout.is_open())
    {
        cout << "writeBackLine: could not open ram.txt\n";
        return false;
    }

    string row;
    long long int rowAddr = 0;
    while (getline(fin, row))
    {
        auto it = patchMap.find(rowAddr);
        if (it != patchMap.end())
        {
            // Row format: "xADDR BBBBBBBB"  — replace just the data field.
            size_t spacePos = row.find(' ');
            if (spacePos != string::npos)
                row = row.substr(0, spacePos + 1) + it->second;
            cout << "writeBackLine: patching byte " << rowAddr
                 << " -> " << it->second << "\n";
        }
        fout << row << "\n";
        ++rowAddr;
    }

    fin.close();
    fout.close();

    if (remove(ramPath.c_str()) != 0 || rename(tmpPath.c_str(), ramPath.c_str()) != 0)
    {
        cout << "writeBackLine: file rename failed\n";
        return false;
    }

    line->invalidate();
    cout << "writeBackLine: line " << lineIdx << " written back and invalidated\n";
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// evictCache
//
// Scans all cache lines.  Every line that is valid AND dirty is flushed to
// ram.txt via writeBackLine() and then invalidated.  Clean lines are left
// untouched.
//
// Returns the number of dirty lines that were written back.
// ─────────────────────────────────────────────────────────────────────────────
int Controller::evictCache()
{
    int nLines     = cache->getNLines();
    int writtenBack = 0;

    cout << "evictCache: scanning " << nLines << " lines...\n";

    for (int i = 0; i < nLines; ++i)
    {
        CacheLine* line = (*cache)[i];

        if (line->isValid() && line->isDirty())
        {
            cout << "evictCache: dirty line " << i
                 << " (tag=" << line->getTag() << ") — writing back\n";

            if (writeBackLine(i))
                ++writtenBack;
            else
                cout << "evictCache: write-back of line " << i << " FAILED\n";
        }
        else
        {
            cout << "evictCache: line " << i
                 << (line->isValid() ? " — clean, skipping" : " — invalid, skipping")
                 << "\n";
        }
    }

    cout << "evictCache: done — " << writtenBack << " line(s) written back\n";
    return writtenBack;
}
