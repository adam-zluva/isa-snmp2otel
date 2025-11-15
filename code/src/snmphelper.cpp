/// ISA ZS 2025
/// xzluva01

#include <stdexcept>
#include <sstream>
#include <iomanip>

#include "snmphelper.hpp"
#include "encoder.hpp"
#include "decoder.hpp"
#include "tags.hpp"
#include "utils.hpp"

std::vector<uint8_t> SNMPHelper::buildSNMPGet(const std::string& community, uint32_t requestId, const std::vector<std::string>& oids)
{
    Encoder enc;

    // SNMP Message ::= SEQUENCE {
    //   version   INTEGER (1 = v2c)
    //   community OCTET STRING
    //   data      GetRequest-PDU
    // }

    enc.startSequence(TAG_SEQUENCE);
    enc.writeInteger(1);
    enc.writeOctetString(community);

    // GetRequest-PDU ::= [0] IMPLICIT SEQUENCE {
    //   request-id      INTEGER
    //   error-status    INTEGER (0)
    //   error-index     INTEGER (0)
    //   variable-bindings SEQUENCE OF VarBind
    // }

    enc.startSequence(TAG_GET_RQ_PDU);
    enc.writeInteger(requestId);
    enc.writeInteger(0);
    enc.writeInteger(0);

    // VarBindList ::= SEQUENCE OF VarBind
    enc.startSequence(TAG_SEQUENCE);

    for (const auto& oid : oids)
    {
        // VarBind ::= SEQUENCE {
        //   name  OBJECT IDENTIFIER,
        //   value NULL
        // }
        enc.startSequence(TAG_SEQUENCE);
        enc.writeOID(oid);
        enc.writeNull();
        enc.endSequence();
    }

    enc.endSequence();
    enc.endSequence();
    enc.endSequence();

    return enc.data();
}

SNMPResponse SNMPHelper::decodeResponse(const std::vector<uint8_t>& data)
{
    Decoder root(data);

    uint8_t tag;
    std::vector<uint8_t> val;

    // Outer message sequence
    root.nextTLV(tag, val);
    if (tag != TAG_SEQUENCE)
        throw std::runtime_error("SNMP: expected outer SEQUENCE");

    Decoder msgDec(val);

    // version
    uint64_t version = msgDec.readInteger();
    (void)version; // currently unused, but consumed

    // community
    std::string community = msgDec.readOctetString();
    (void)community;

    // PDU (GetResponse)
    msgDec.nextTLV(tag, val);
    if (tag != TAG_GET_RSP_PDU)
        throw std::runtime_error("SNMP: expected GET RESPONSE PDU");

    Decoder dec(val);

    SNMPResponse response;

    response.requestId = dec.readInteger();
    response.errorStatus = dec.readInteger();
    response.errorIndex = dec.readInteger();

    // VarBindList ::= SEQUENCE OF VarBind
    dec.nextTLV(tag, val);
    if (tag != TAG_SEQUENCE)
        throw std::runtime_error("SNMP: expected VarBindList SEQUENCE");

    Decoder vbListDec(val);
    while (vbListDec.hasRemaining())
    {
        vbListDec.nextTLV(tag, val);
        if (tag != TAG_SEQUENCE)
            throw std::runtime_error("SNMP: expected VarBind SEQUENCE");

        Decoder vbDec(val);
        SNMPVarBind vb;

        vb.oid = vbDec.readOID(); // name

        // value: read next TLV raw so we can inspect tag
        uint8_t vtag;
        std::vector<uint8_t> vval;
        vbDec.nextTLV(vtag, vval);
        vb.tag = vtag;

        if (vtag == TAG_NULL)
        {
            vb.value = std::string();
        } else if (vtag == TAG_OCTET_STRING)
        {
            vb.value = std::string(vval.begin(), vval.end());
        } else if (vtag == TAG_INTEGER || vtag == TAG_GAUGE || vtag == TAG_TIMETICKS)
        {
            vb.value = std::to_string(Decoder::decodeIntegerValue(vval));
        } else if (vtag == TAG_OID)
        {
            vb.value = Decoder::decodeOIDValue(vval);
        } else
        {
            // fallback: hex representation
            auto hexString = Utils::hexVectorToString(vval);
            vb.value = hexString;
        }

        response.varbinds.push_back(vb);
    }

    return response;
}

// Stringify helpers
std::string SNMPVarBind::toString() const
{
    std::ostringstream ss;
    ss << oid << " = ";

    if (tag == TAG_NULL)
    {
        ss << "NULL";
    } else if (tag == TAG_OCTET_STRING)
    {
        ss << '"' << value << '"';
    } else if (tag == TAG_INTEGER || tag == TAG_GAUGE || tag == TAG_TIMETICKS || tag == TAG_OID)
    {
        ss << value;
    } else if (tag == TAG_NO_SUCH_OBJECT)
    {
        ss << "noSuchObject";
    } else
    {
        // edge case print tag and hex value
        ss << "[tag=0x" << Utils::intToHexString(tag) << "] " << value;
    }

    return ss.str();
}

std::string SNMPResponse::toString() const
{
    std::ostringstream ss;
    ss << "requestId=" << requestId
       << " errorStatus=" << errorStatus
       << " errorIndex=" << errorIndex << '\n';

    for (const auto &vb : varbinds)
    {
        ss << "  " << vb.toString() << '\n';
    }

    return ss.str();
}
