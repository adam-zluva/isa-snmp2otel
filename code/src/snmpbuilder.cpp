#include "snmpbuilder.hpp";

#include "encoder.hpp"

std::vector<uint8_t> SNMPBuilder::buildSNMPGet(const std::string& community, uint32_t requestId, const std::vector<std::string>& oids)
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

    // GetRequest-PDU (tag 0xA0)
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

    for (const auto& oid : oids) {
        // VarBind ::= SEQUENCE {
        //   name  OBJECT IDENTIFIER,
        //   value ANY -- pro Get je to NULL
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
