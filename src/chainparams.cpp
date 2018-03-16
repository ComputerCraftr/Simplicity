// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assert.h"

#include "chainparams.h"
#include "main.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

//
// Main network
//

// Convert the pnSeeds array into usable address objects.
static void convertSeeds(std::vector<CAddress> &vSeedsOut, const unsigned int *data, unsigned int count, int port)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int k = 0; k < count; ++k)
    {
        struct in_addr ip;
        unsigned int i = data[k], t;
        
        // -- convert to big endian
        t =   (i & 0x000000ff) << 24u
            | (i & 0x0000ff00) << 8u
            | (i & 0x00ff0000) >> 8u
            | (i & 0xff000000) >> 24u;
        
        memcpy(&ip, &t, sizeof(ip));
        
        CAddress addr(CService(ip, port));
        addr.nTime = GetTime()-GetRand(nOneWeek)-nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

class CMainParams : public CChainParams {
public:
    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xb3;
        pchMessageStart[1] = 0x07;
        pchMessageStart[2] = 0x9a;
        pchMessageStart[3] = 0x1e;
        vAlertPubKey = ParseHex("04eb9fd13c016ed9a2f222989769417ed3a16e6fbf3bd55023679be17f0bab053cd9f161528241bc9a2894ad5ebbbd551be1a4bd2d10cdb679228c91e26e26900e");
        nDefaultPort = 11957;
        nRPCPort = 11958;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);

        const char* pszTimestamp = "http://www.bbc.co.uk/news/world-us-canada-42926976";  // Trump Russia: Democrats say firing special counsel could cause crisis
        std::vector<CTxIn> vin;
        vin.resize(1);
        vin[0].scriptSig = CScript() << 4867816 << CBigNum(42) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        std::vector<CTxOut> vout;
        vout.resize(1);
        vout[0].SetEmpty();
        CTransaction txNew(1, 1517690700, vin, vout, 0);
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1517690700;
        genesis.nBits    = 0x1f00ffff; 
        genesis.nNonce   = 561379;

        hashGenesisBlock = genesis.GetHash();

        assert(hashGenesisBlock == uint256("0xf4bbfc518aa3622dbeb8d2818a606b82c2b8b1ac2f28553ebdb6fc04d7abaccf"));
        assert(genesis.hashMerkleRoot == uint256("0x40bdd3d5ae84b91a71190094a82948400eb3356e87c5376b64d79509cf552d84"));

        
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,18);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,59);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,93);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x44)(0xD5)(0xBC).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x44)(0xF0)(0xA3).convert_to_container<std::vector<unsigned char> >();

        convertSeeds(vFixedSeeds, pnSeed, ARRAYLEN(pnSeed), nDefaultPort);

        nPOSStartBlock = 1;
    }

    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }

    virtual const vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};
static CMainParams mainParams;


//
// Testnet
//

class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xf1;
        pchMessageStart[1] = 0xe3;
        pchMessageStart[2] = 0xdc;
        pchMessageStart[3] = 0xc6;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);
        vAlertPubKey = ParseHex("04eb9fd13c016ed9a2f222989769417ed3a16e6fbf3bd55023679be17f0bab053cd9f161528241bc9a2894ad5ebbbd551be1a4bd2d10cdb679228c91e26e26900e");
        nDefaultPort = 21957;
        nRPCPort = 21958;
        strDataDir = "testnet";

        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nBits  = 51214089; 
        genesis.nNonce = 93481;

        //assert(hashGenesisBlock == uint256(""));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,27);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,95);
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1,98);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x05)(0x55)(0xCF)(0xB1).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x05)(0x55)(0xD4)(0x7A).convert_to_container<std::vector<unsigned char> >();

        convertSeeds(vFixedSeeds, pnTestnetSeed, ARRAYLEN(pnTestnetSeed), nDefaultPort);

        nPOSStartBlock = 1;
    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;


static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

void SelectParams(CChainParams::Network network) {
    switch (network) {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine() {
    
    bool fTestNet = GetBoolArg("-testnet", false);
    
    if (fTestNet) {
        SelectParams(CChainParams::TESTNET);
    } else {
        SelectParams(CChainParams::MAIN);
    }
    return true;
}
