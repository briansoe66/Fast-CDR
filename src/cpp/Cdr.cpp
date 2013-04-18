#include "cpp/Cdr.h"
#include <string.h>

using namespace eProsima;

CDR::CDR(CDRBuffer &cdrBuffer, const CdrType cdrType) : m_cdrBuffer(cdrBuffer),
    m_cdrType(cdrType), m_plFlag(DDS_CDR_WITHOUT_PL), m_options(0)
{
}

bool CDR::read_encapsulation()
{
    bool returnedValue = true;
    uint8_t dummy, encapsulationKind;

    // If it is DDS_CDR, the first step is to get the dummy byte.
    if(m_cdrType == DDS_CDR)
    {
        returnedValue &= (*this) >> dummy;
    }

    // Get the ecampsulation byte.
    returnedValue &= (*this) >> encapsulationKind;

    if(returnedValue)
    {
        // If it is a different endianness, make changes.
        if(m_cdrBuffer.m_endianness != (encapsulationKind & 0x1))
        {
            m_cdrBuffer.m_swapBytes = !m_cdrBuffer.m_swapBytes;
            m_cdrBuffer.m_endianness = encapsulationKind;
        }

        // If it is DDS_CDR type, view if contains a parameter list.
        if(encapsulationKind & DDS_CDR_WITH_PL)
        {
            if(m_cdrType == DDS_CDR)
            {
                m_plFlag = DDS_CDR_WITH_PL;
            }
            else
            {
                returnedValue = false;
            }
        }
    }

    if(returnedValue && (m_cdrType == DDS_CDR))
        returnedValue &= (*this) >> m_options;

    return returnedValue;
}

CDR::DDSCdrPlFlag CDR::getDDSCdrPlFlag() const
{
    return m_plFlag;
}

uint16_t CDR::getDDSCdrOptions() const
{
    return m_options;
}

bool CDR::jump(uint32_t numBytes)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(numBytes)))
    {
        m_cdrBuffer.m_currentPosition += numBytes;
        returnedValue = true;
    }

    return returnedValue;
}

void CDR::resetAlignment()
{
	m_cdrBuffer.resetAlign();
}

char* CDR::getCurrentPosition()
{
    return m_cdrBuffer.m_currentPosition;
}

CDRBuffer::State CDR::getState() const
{
    return CDRBuffer::State(m_cdrBuffer);
}

void CDR::setState(CDRBuffer::State &state)
{
    m_cdrBuffer.m_currentPosition = state.m_currentPosition;
    m_cdrBuffer.m_bufferRemainLength = state.m_bufferRemainLength;
    m_cdrBuffer.m_alignPosition = state.m_alignPosition;
    m_cdrBuffer.m_swapBytes = state.m_swapBytes;
    m_cdrBuffer.m_lastDataSize = state.m_lastDataSize;
}

bool CDR::serialize(const char char_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(char_t)) || m_cdrBuffer.resize(sizeof(char_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        *m_cdrBuffer.m_currentPosition++ = char_t;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(char_t);
        return true;
    }

    return false;
}

bool CDR::serialize(const int16_t short_t)
{
    size_t align = m_cdrBuffer.align(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&short_t);

        if(m_cdrBuffer.m_swapBytes)
        {    
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(short_t));
            m_cdrBuffer.m_currentPosition += sizeof(short_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const int16_t short_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serialize(short_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const int32_t long_t)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&long_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(long_t));
            m_cdrBuffer.m_currentPosition += sizeof(long_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const int32_t long_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serialize(long_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const int64_t longlong_t)
{
    size_t align = m_cdrBuffer.align(sizeof(longlong_t));
    size_t sizeAligned = sizeof(longlong_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&longlong_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[7];
            *m_cdrBuffer.m_currentPosition++ = dst[6];
            *m_cdrBuffer.m_currentPosition++ = dst[5];
            *m_cdrBuffer.m_currentPosition++ = dst[4];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(longlong_t));
            m_cdrBuffer.m_currentPosition += sizeof(longlong_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const int64_t longlong_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serialize(longlong_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const float float_t)
{
    size_t align = m_cdrBuffer.align(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&float_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(float_t));
            m_cdrBuffer.m_currentPosition += sizeof(float_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const float float_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serialize(float_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const double double_t)
{
    size_t align = m_cdrBuffer.align(sizeof(double_t));
    size_t sizeAligned = sizeof(double_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&double_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[7];
            *m_cdrBuffer.m_currentPosition++ = dst[6];
            *m_cdrBuffer.m_currentPosition++ = dst[5];
            *m_cdrBuffer.m_currentPosition++ = dst[4];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(double_t));
            m_cdrBuffer.m_currentPosition += sizeof(double_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const double double_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serialize(double_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const bool bool_t)
{
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(uint8_t)) || m_cdrBuffer.resize(sizeof(uint8_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        if(bool_t)
            value = 1;
        *m_cdrBuffer.m_currentPosition++ = value;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(uint8_t);

        return true;
    }

    return false;
}

bool CDR::serialize(const std::string &string_t)
{
    bool returnedValue = true;
    uint32_t length = (uint32_t)string_t.length();

    returnedValue &= *this << length;

    if(length > 0 && (m_cdrBuffer.checkSpace(length) || m_cdrBuffer.resize(length)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        memcpy(m_cdrBuffer.m_currentPosition, string_t.c_str(), length);
        m_cdrBuffer.m_currentPosition += length;
        m_cdrBuffer.m_bufferRemainLength -= length;
    }

    return returnedValue;
}

bool CDR::serialize(const std::string &string_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serialize(string_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

int CDR::serialize(User_CString *userString, size_t userStringLength, User_CString_FuncGetData funcGetData)
{
    uint32_t length = (uint32_t)userStringLength;
    char *data = NULL;

    if(*this << length)
    {
        if(length > 0)
        {
            if(m_cdrBuffer.checkSpace(length) || m_cdrBuffer.resize(length))
            {
                if((data = funcGetData(userString)) != NULL)
                {
                    // Save last datasize.
                    m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

                    memcpy(m_cdrBuffer.m_currentPosition, data, length);
                    m_cdrBuffer.m_currentPosition += length;
                    m_cdrBuffer.m_bufferRemainLength -= length;
                    return 0;
                }
            }
        }
        else
            return 0;
    }

    return -1;
}

int CDR::serialize(User_CString *userString, size_t userStringSize, User_CString_FuncGetData funcGetData, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    int returnedValue = serialize(userString, userStringSize, funcGetData) ? 0 : -1;
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        memcpy(m_cdrBuffer.m_currentPosition, char_t, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;
        return true;
    }

    return false;
}

bool CDR::serializeArray(const int16_t *short_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&short_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*short_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, short_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeArray(short_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const int32_t *long_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&long_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*long_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, long_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeArray(long_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const int64_t *longlong_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(longlong_t));
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&longlong_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*longlong_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[7];
                *m_cdrBuffer.m_currentPosition++ = dst[6];
                *m_cdrBuffer.m_currentPosition++ = dst[5];
                *m_cdrBuffer.m_currentPosition++ = dst[4];
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, longlong_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeArray(longlong_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const float *float_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&float_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*float_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, float_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const float *float_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeArray(float_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const double *double_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(double_t));
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&double_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*double_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[7];
                *m_cdrBuffer.m_currentPosition++ = dst[6];
                *m_cdrBuffer.m_currentPosition++ = dst[5];
                *m_cdrBuffer.m_currentPosition++ = dst[4];
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, double_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const double *double_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeArray(double_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeSequence(const char *char_t, size_t numElements)
{
    bool returnedValue = false;

    if(*this << (int32_t)numElements)
    {
        returnedValue = serializeArray(char_t, numElements);
    }

    return returnedValue;
}

bool CDR::serializeSequence(const char *char_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeSequence(char_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeSequence(const int16_t *short_t, size_t numElements)
{
    bool returnedValue = false;

    if(*this << (int32_t)numElements)
    {
        returnedValue = serializeArray(short_t, numElements);
    }

    return returnedValue;
}

bool CDR::serializeSequence(const int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeSequence(short_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeSequence(const int32_t *long_t, size_t numElements)
{
    bool returnedValue = false;

    if(*this << (int32_t)numElements)
    {
        returnedValue = serializeArray(long_t, numElements);
    }

    return returnedValue;
}

bool CDR::serializeSequence(const int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeSequence(long_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeSequence(const int64_t *longlong_t, size_t numElements)
{
    bool returnedValue = false;

    if(*this << (int32_t)numElements)
    {
        returnedValue = serializeArray(longlong_t, numElements);
    }

    return returnedValue;
}

bool CDR::serializeSequence(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeSequence(longlong_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeSequence(const float *float_t, size_t numElements)
{
    bool returnedValue = false;

    if(*this << (int32_t)numElements)
    {
        returnedValue = serializeArray(float_t, numElements);
    }

    return returnedValue;
}

bool CDR::serializeSequence(const float *float_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeSequence(float_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeSequence(const double *double_t, size_t numElements)
{
    bool returnedValue = false;

    if(*this << (int32_t)numElements)
    {
        returnedValue = serializeArray(double_t, numElements);
    }

    return returnedValue;
}

bool CDR::serializeSequence(const double *double_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = serializeSequence(double_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(char &char_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(char_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        char_t = *m_cdrBuffer.m_currentPosition++;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(char_t);
        return true;
    }

    return false;
}

bool CDR::deserialize(int16_t &short_t)
{
    size_t align = m_cdrBuffer.align(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        // Align
        m_cdrBuffer.makeAlign(align);
        char *dst = reinterpret_cast<char*>(&short_t);

        if(m_cdrBuffer.m_swapBytes)
        {    
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(short_t));
            m_cdrBuffer.m_currentPosition += sizeof(short_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(int16_t &short_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserialize(short_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(int32_t &long_t)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        // Align.
        m_cdrBuffer.makeAlign(align);
        char *dst = reinterpret_cast<char*>(&long_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(long_t));
            m_cdrBuffer.m_currentPosition += sizeof(long_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(int32_t &long_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserialize(long_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(int64_t &longlong_t)
{
    size_t align = m_cdrBuffer.align(sizeof(longlong_t));
    size_t sizeAligned = sizeof(longlong_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);

        // Align.
        m_cdrBuffer.makeAlign(align);
        char *dst = reinterpret_cast<char*>(&longlong_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[7] = *m_cdrBuffer.m_currentPosition++;
            dst[6] = *m_cdrBuffer.m_currentPosition++;
            dst[5] = *m_cdrBuffer.m_currentPosition++;
            dst[4] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(longlong_t));
            m_cdrBuffer.m_currentPosition += sizeof(longlong_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(int64_t &longlong_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserialize(longlong_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(float &float_t)
{
    size_t align = m_cdrBuffer.align(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);

        // Align.
        m_cdrBuffer.makeAlign(align);
        char *dst = reinterpret_cast<char*>(&float_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(float_t));
            m_cdrBuffer.m_currentPosition += sizeof(float_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(float &float_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserialize(float_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(double &double_t)
{
    size_t align = m_cdrBuffer.align(sizeof(double_t));
    size_t sizeAligned = sizeof(double_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);

        // Align.
        m_cdrBuffer.makeAlign(align);
        char *dst = reinterpret_cast<char*>(&double_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[7] = *m_cdrBuffer.m_currentPosition++;
            dst[6] = *m_cdrBuffer.m_currentPosition++;
            dst[5] = *m_cdrBuffer.m_currentPosition++;
            dst[4] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(double_t));
            m_cdrBuffer.m_currentPosition += sizeof(double_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(double &double_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserialize(double_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(bool &bool_t)
{
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(uint8_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        value = *m_cdrBuffer.m_currentPosition++;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(uint8_t);

        if(value == 1)
        {
            bool_t = true;
            return true;
        }
        else if(value == 0)
        {
            bool_t = false;
            return true;
        }
    }

    return false;
}

bool CDR::deserialize(std::string &string_t)
{
    bool returnedValue = true;
    uint32_t length = 0;

    returnedValue &= *this >> length;

    if(length == 0)
    {
        string_t = "";
    }
    else if(returnedValue &= m_cdrBuffer.checkSpace(length))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        string_t = std::string(m_cdrBuffer.m_currentPosition, length - (m_cdrBuffer.m_currentPosition[length-1] == '\0' ? 1 : 0));
        m_cdrBuffer.m_currentPosition += length;
        m_cdrBuffer.m_bufferRemainLength -= length;
    }

    return returnedValue;
}

bool CDR::deserialize(std::string &string_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserialize(string_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

int CDR::deserialize(User_CString *userString, size_t userStringSize, User_CString_FuncGetData funcGetData, User_CString_FuncAllocator funcAllocator)
{
    uint32_t length = 0;
    char *data = NULL;

    if(*this >> length)
    {
        if(length == 0)
        {
            if((userStringSize > 0) || (funcAllocator(userString, 1) == 0))
            {
                if((data = funcGetData(userString)) != NULL)
                {
                    data[0] = '\0';
                    return 0;
                }
            }
        }
        else if(m_cdrBuffer.checkSpace(length))
        {
            uint32_t addition = length + (m_cdrBuffer.m_currentPosition[length-1] == '\0' ? 0 : 1);

            if((userStringSize >= addition) || (funcAllocator(userString, addition) == 0))
            {
                if((data = funcGetData(userString)) != NULL)
                {
                    // Save last datasize.
                    m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

                    memcpy(data, m_cdrBuffer.m_currentPosition, length);
                    data[addition-1] = '\0';
                    m_cdrBuffer.m_currentPosition += length;
                    m_cdrBuffer.m_bufferRemainLength -= length;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int CDR::deserialize(User_CString *userString, size_t userStringSize, User_CString_FuncGetData funcGetData, User_CString_FuncAllocator funcAllocator, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    int returnedValue = deserialize(userString, userStringSize, funcGetData, funcAllocator) ? 0 : -1;
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        memcpy(char_t, m_cdrBuffer.m_currentPosition, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;
        return true;
    }

    return false;
}

bool CDR::deserializeArray(int16_t *short_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&short_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*short_t))
            {
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(short_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeArray(short_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(int32_t *long_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&long_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*long_t))
            {
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(long_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeArray(long_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(int64_t *longlong_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(longlong_t));
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&longlong_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*longlong_t))
            {
                dst[7] = *m_cdrBuffer.m_currentPosition++;
                dst[6] = *m_cdrBuffer.m_currentPosition++;
                dst[5] = *m_cdrBuffer.m_currentPosition++;
                dst[4] = *m_cdrBuffer.m_currentPosition++;
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(longlong_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeArray(longlong_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(float *float_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&float_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*float_t))
            {
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(float_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(float *float_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeArray(float_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(double *double_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(double_t));
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&double_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*double_t))
            {
                dst[7] = *m_cdrBuffer.m_currentPosition++;
                dst[6] = *m_cdrBuffer.m_currentPosition++;
                dst[5] = *m_cdrBuffer.m_currentPosition++;
                dst[4] = *m_cdrBuffer.m_currentPosition++;
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(double_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(double *double_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeArray(double_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(char *char_t, size_t maxNumElements, size_t &numElements)
{
    bool returnedValue = false;
    CDRBuffer::State state(m_cdrBuffer);

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements)
        {
            returnedValue = deserializeArray(char_t, numElements);
        }
        else
        {
            setState(state);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(char *char_t, size_t maxNumElements, size_t &numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(char_t, maxNumElements, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(char *char_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator)
{
    bool returnedValue = false;

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements || (sequenceFuncAllocator != NULL && sequenceFuncAllocator((char**)&char_t, maxNumElements, numElements)))
        {
            returnedValue = deserializeArray(char_t, numElements);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(char *char_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(char_t, maxNumElements, numElements, sequenceFuncAllocator);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(int16_t *short_t, size_t maxNumElements, size_t &numElements)
{
    bool returnedValue = false;
    CDRBuffer::State state(m_cdrBuffer);

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements)
        {
            returnedValue = deserializeArray(short_t, numElements);
        }
        else
        {
            setState(state);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(int16_t *short_t, size_t maxNumElements, size_t &numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(short_t, maxNumElements, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(int16_t *short_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator)
{
    bool returnedValue = false;

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements || (sequenceFuncAllocator != NULL && sequenceFuncAllocator((char**)&short_t, maxNumElements, numElements)))
        {
            returnedValue = deserializeArray(short_t, numElements);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(int16_t *short_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(short_t, maxNumElements, numElements, sequenceFuncAllocator);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(int32_t *long_t, size_t maxNumElements, size_t &numElements)
{
    bool returnedValue = false;
    CDRBuffer::State state(m_cdrBuffer);

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements)
        {
            returnedValue = deserializeArray(long_t, numElements);
        }
        else
        {
            setState(state);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(int32_t *long_t, size_t maxNumElements, size_t &numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(long_t, maxNumElements, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(int32_t *long_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator)
{
    bool returnedValue = false;

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements || (sequenceFuncAllocator != NULL && sequenceFuncAllocator((char**)&long_t, maxNumElements, numElements)))
        {
            returnedValue = deserializeArray(long_t, numElements);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(int32_t *long_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(long_t, maxNumElements, numElements, sequenceFuncAllocator);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(int64_t *longlong_t, size_t maxNumElements, size_t &numElements)
{
    bool returnedValue = false;
    CDRBuffer::State state(m_cdrBuffer);

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements)
        {
            returnedValue = deserializeArray(longlong_t, numElements);
        }
        else
        {
            setState(state);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(int64_t *longlong_t, size_t maxNumElements, size_t &numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(longlong_t, maxNumElements, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(int64_t *longlong_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator)
{
    bool returnedValue = false;

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements || (sequenceFuncAllocator != NULL && sequenceFuncAllocator((char**)&longlong_t, maxNumElements, numElements)))
        {
            returnedValue = deserializeArray(longlong_t, numElements);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(int64_t *longlong_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(longlong_t, maxNumElements, numElements, sequenceFuncAllocator);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(float *float_t, size_t maxNumElements, size_t &numElements)
{
    bool returnedValue = false;
    CDRBuffer::State state(m_cdrBuffer);

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements)
        {
            returnedValue = deserializeArray(float_t, numElements);
        }
        else
        {
            setState(state);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(float *float_t, size_t maxNumElements, size_t &numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(float_t, maxNumElements, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(float *float_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator)
{
    bool returnedValue = false;

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements || (sequenceFuncAllocator != NULL && sequenceFuncAllocator((char**)&float_t, maxNumElements, numElements)))
        {
            returnedValue = deserializeArray(float_t, numElements);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(float *float_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(float_t, maxNumElements, numElements, sequenceFuncAllocator);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(double *double_t, size_t maxNumElements, size_t &numElements)
{
    bool returnedValue = false;
    CDRBuffer::State state(m_cdrBuffer);

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements)
        {
            returnedValue = deserializeArray(double_t, numElements);
        }
        else
        {
            setState(state);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(double *double_t, size_t maxNumElements, size_t &numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(double_t, maxNumElements, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeSequence(double *double_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator)
{
    bool returnedValue = false;

    if(*this >> (int32_t&)numElements)
    {
        if(numElements <= maxNumElements || (sequenceFuncAllocator != NULL && sequenceFuncAllocator((char**)&double_t, maxNumElements, numElements)))
        {
            returnedValue = deserializeArray(double_t, numElements);
        }
    }

    return returnedValue;
}

bool CDR::deserializeSequence(double *double_t, size_t maxNumElements, size_t &numElements, SequenceFuncAllocator sequenceFuncAllocator, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    bool returnedValue = deserializeSequence(double_t, maxNumElements, numElements, sequenceFuncAllocator);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}
