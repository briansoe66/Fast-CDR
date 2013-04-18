#include "cpp/CdrBuffer.h"

#include <malloc.h>
#include <cstddef>

#define BUFFER_START_LENGTH 200

namespace eProsima
{
#if defined(__LITTLE_ENDIAN__)
    const CDRBuffer::Endianness CDRBuffer::DEFAULT_ENDIAN = LITTLE_ENDIANNESS;
#elif defined (__BIG_ENDIAN__)
    const CDRBuffer::Endianness CDRBuffer::DEFAULT_ENDIAN = BIG_ENDIANNESS;
#endif

    CDRBuffer::State::State(CDRBuffer &cdrBuffer) : m_bufferRemainLength(cdrBuffer.m_bufferRemainLength), m_currentPosition(cdrBuffer.m_currentPosition),
                 m_alignPosition(cdrBuffer.m_alignPosition), m_swapBytes(cdrBuffer.m_swapBytes), m_lastDataSize(cdrBuffer.m_lastDataSize) {}

    CDRBuffer::CDRBuffer(const Endianness endianness) : m_buffer(NULL),
        m_bufferSize(0), m_bufferRemainLength(0), m_currentPosition(NULL), m_alignPosition(NULL), m_endianness(endianness),
        m_swapBytes(endianness == DEFAULT_ENDIAN ? false : true), m_lastDataSize(0), m_internalBuffer(true)
    {
    }

    CDRBuffer::CDRBuffer(char* const buffer, const size_t bufferSize, const Endianness endianness) : m_buffer(buffer),
        m_bufferSize(bufferSize), m_bufferRemainLength(bufferSize), m_currentPosition(buffer), m_alignPosition(buffer), m_endianness(endianness),
        m_swapBytes(endianness == DEFAULT_ENDIAN ? false : true), m_lastDataSize(0), m_internalBuffer(false)
    {
    }

    void CDRBuffer::reset()
    {
        m_bufferRemainLength = m_bufferSize;
        m_currentPosition = m_buffer;
        m_alignPosition = m_buffer;
        m_swapBytes = m_endianness == DEFAULT_ENDIAN ? false : true;
        m_lastDataSize = 0;
    }

    bool CDRBuffer::resize(size_t minSizeInc)
    {
        size_t incBufferSize = BUFFER_START_LENGTH;

        if(m_internalBuffer)
        {
            if(minSizeInc > BUFFER_START_LENGTH)
            {
                incBufferSize = minSizeInc;
            }

            if(m_buffer == NULL)
            {
                m_bufferSize = incBufferSize;

                m_buffer = (char*)malloc(m_bufferSize);

                if(m_buffer != NULL)
                {
                    m_currentPosition = m_buffer;
                    m_alignPosition = m_buffer;
                    m_bufferRemainLength = incBufferSize;
                    return true;
                }
            }
            else
            {
                ptrdiff_t currentPositionDiff = m_currentPosition - m_buffer;
                ptrdiff_t alignPositionDiff = m_alignPosition - m_buffer;
                m_bufferSize += incBufferSize;

                m_buffer = (char*)realloc(m_buffer, m_bufferSize);

                if(m_buffer != NULL)
                {
                    m_currentPosition = m_buffer + currentPositionDiff;
                    m_alignPosition = m_buffer + alignPositionDiff;
                    m_bufferRemainLength += incBufferSize;
                    return true;
                }
            }
        }

        return false;
    }
};
