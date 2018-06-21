// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

#include <stdint.h>

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64_t nTimeLastCheckpoint;
        int64_t nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    bool fEnabled = true;

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0, Params().HashGenesisBlock() )
        ( 57, uint256("0x1e3675f91dbf0a6ebeb3e9b151f43e6423d2be8737a8c6ce64874b0f83158ad9"))
        ( 122, uint256("0x420b61a37d052e1b5fea6decd82ee2d78cefaa59f9e7dc55f4318334b4dfc0f2"))
        ( 526, uint256("0x33c208288fe6747122b97fc6d79bc5d22ae6a7f8685bf78f4482238d2e8976f8"))
        ( 1207, uint256("0x91e3810178c82b0582f6606609fc2348fd48b3ab10c7067d936c877759fc034f"))
        ( 2840, uint256("0x9379716dd09d4f7da008605e7e9641ee52e7daad643287a2c5231110b2816cd0"))
        ( 4604, uint256("0xfe48209b2c7498f2d30706d843ec503fc7e230d5f2532f50df690fd6b1b0ac98"))
        ( 7170, uint256("0x52ff5378e6e7e4652f757910bd7db4bd3b122a4a773e80514cef5d95d41639c4"))
        ( 8862, uint256("0xf5051998f1d35d945d1f45d6f827cd09547ced272d8ca3c2de97af5cfb478ed2"))
        ( 13555, uint256("0xffc2cadd9683c118346f6e0e52d469bf4cd07c6beab0e9a9e35ce576b8059977"))
        ( 16932, uint256("0x00f683e5963760f1409f57d6f5c221ca4bbe9fedd054a71aed896f74c73ec0bd"))
        ( 19887, uint256("0x59433846a94700fa9c120768f2f2d0525e2b0f2311fb5451b5282b83b44e655a"))
        ( 22677, uint256("0x20abf525927417e613d2abd4edd518767588ac26dff9aabe10e7d17756839033"))
        ( 26605, uint256("0xcc96d7b1516aa6733d2bfccbb8dffb94f63bf1d64e9e4decb7b76d5256958a70"))
        ( 35119, uint256("0x28e91350773eb74782021a2a49d8732ae0036e63b43627a513b4abd706c6885e"))
        ( 43107, uint256("0xf6bf5f78c30c4e9f81b07be1f6f6424749a4f8ec0a6f4971ed5ba2753606b991"))
        ( 46538, uint256("0xa7608f08e298b8b9a384588f6a5c978c18131acd25eff482006c85f4a0957350"))
        ( 50844, uint256("0x45d5b87278e0231a5d5d695d4fc4f488a15d3a03ebda8487f5daba3ec98ec4da"))
        ( 77776, uint256("0x8d0d4799be45e43afe931b8b58d223f20ed01d8b205d542f758474c39c1be481"))
        ( 80000, uint256("0xb1599cb3c009249b07fd5f69c2e45482ec49b4b04ad6842e92f09583eee5cc10"))
        ( 84150, uint256("0xeb4a5ae490acfb87dbe112e53a6b586b96b300ecfce7d59827ff020138926ad9"))
        ( 90000, uint256("0xaaf5e94168499444193a950adf25ceccc7d2a8cd3a74db0cc1fd2485da6d6662"))
        ( 95000, uint256("0x6db2473b889039fb9a44293716a8c16ce18293d558a3fbb62eab81b6fd5d05b7"))
        ( 100000, uint256("0x9c8f67b0d656a451250b1f4e1fca9980e23ae5eb2d70e0798b76ea4c30e63bad"))
        ( 110000, uint256("0x0f3cde93b1e79ceedbc9682f2d3a3af37b38f71c1caef74e09d3a9565036891f"))
        ( 130000, uint256("0x9af8f639d1acbcf525afa3646947f1d1f615c8d628f57a879f048a0436b61037"))
        ( 230000, uint256("0xb861552883a868ec34f91ad4c1893201466771423c065c19eb1c8bbcee60c02a"))
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1528242462, // * UNIX timestamp of last checkpoint block
        1000,   // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        10     // * estimated number of transactions per day after checkpoint
    };

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1528242462,
        1000,
        10
    };

    const CCheckpointData &Checkpoints() {
        if (Params().NetworkID() == CChainParams::TESTNET)
            return dataTestnet;
        else if (Params().NetworkID() == CChainParams::MAIN)
            return data;
        else
            return dataRegtest;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (!fEnabled)
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64_t nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkpoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (!fEnabled)
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (!fEnabled)
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
