#ifndef __RANDSOURCE_H
#define __RANDSOURCE_H

#include <cstdlib>
#include <vector>

class RandSource;

class RandStream
{
public:
    RandStream(RandSource& randSource, long currStream, size_t currPos, bool sync=false):
        randSource(randSource), currStream(currStream), currPos(currPos), sync(sync)
    {}

    void initseed(unsigned int num) {
        #pragma omp critical
        {
            seed=num;
        }
    }

    unsigned int get()
    {
        unsigned int result;
        #pragma omp critical
        {
            result = rand_r(&seed);
        }
        return result;
    }

    inline double getf()
    {
        return ((double)get() / RAND_MAX);
    }

    friend class RandSource;
  private:
    unsigned int seed;
};

/**
   @class RandSource
   @brief Source of random numbers
   @details Generates streams of random numbers that are then reused.

   @author Wee Sun Lee
   @date 26 October 2009
*/
class RandSource
{
 public:
    RandSource(long numStream, long blockSize = 100):blockSize(blockSize), numStream(numStream), tempStream(*this, 0,0, true)
    {
        seeds.resize(numStream);
        for(int i=0; i < numStream; ++i)
            sources.push_back(RandStream());
        currStream = 0;
        currNum = 0;
    };

    static void init(unsigned seed) { srand(seed); };

    inline unsigned get()
    {
        #pragma omp parallel for
        for(unsigned int i=0; i<numStream; ++i) {
            seeds[i] = (seed ^ i);
            sources[i].initseed(seeds[i]);
        }
        unsigned out = sources[currStream][currNum];
        currNum++;
        return out;
    };

    inline void extend(long iStream)
    {
        for (long i=0; i < blockSize; i++) sources[iStream].push_back(rand());
    }

    inline double getf()
    {
        return ((double)get() / RAND_MAX);
    }

    inline void startStream(long streamNum)
    {
        currNum = 0;
        currStream = streamNum;
    };

    inline void setStreamPos(long streamNum, long pos)
    {
        currNum = pos;
        currStream = streamNum;
    };

    inline long getStreamNum() { return currStream; };
    inline long getPosInStream() { return currNum; };

    inline RandStream& getStream(long numStream, long numPos)
    {
        sources[numStream].initseed(seeds[numStream]);
        for (long i = 0; i < numPos; i++)
            sources[numStream].get();
        return sources[numStream];
    }

    inline RandStream& getStream(long numStream)
    {
        return RandStream(*this, numStream, numPos);
    }


    inline operator RandStream&()
    {
        tempStream.currStream = currStream;
        tempStream.currPos = currNum;
        return tempStream;
    }

    inline void reset()
    {
        for (long i=0; i< numStream; i++)
            sources[i].resize(0);
        for (long j = 0; j < numStream; j++)
            for (long i=0; i< blockSize; i++){
                sources[j].push_back(rand());
            }
        currStream = 0;
        currNum = 0;
    };

    friend class RandStream;
 private:
    long blockSize;
    long numStream;
    long currStream;
    size_t currNum;
    std::vector<RandStream> sources;
    std::vector<unsigned> seeds;
};
#endif //  __RANDSOURCE_H
