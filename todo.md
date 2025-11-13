# Plán Projektu: snmp2otel (Zkrácená verze)

---

## Milník 1: Infrastruktura a CLI

* **M1.1**: **Nastavení** adresářů (`src`, `include`, `obj`) a `Makefile`. ✅
* **M1.2**: **Parsvání CLI** argumentů (všech). ✅
* **M1.3**: **Načítání OID** ze souboru (`oids_file`), filtrace komentářů. ✅
* **M1.4**: **Hlavní smyčka** a implementace časovače (`interval`). ✅

---

## Milník 2: SNMP Klient a Protokol

* **M2.1**: **ASN.1 BER** kódování/dekódování (INTEGER, OCTET STRING, OID, SEQUENCE). ✅
* **M2.2**: **SNMP v2c zpráva** struktura a parsování. ✅
* **M2.3**: **Sestavení GetRequest** PDU pro OID. ✅
* **M2.4**: **UDP Komunikace** (odesílání/příjem na 161). ✅
* **M2.5**: **Retry & Timeout** logika (`-r`, `-T`). ✅
* **M2.6**: **Parsvání GetResponse**, extrakce hodnoty, ověření `.0`.
* **M2.7**: **Zpracování chyb** SNMP (např. `noSuchName`), pokračování.

---

## Milník 3: OpenTelemetry Export (OTLP/HTTP JSON)

* **M3.1**: **OTEL JSON** schéma pro Gauge metriky.
* **M3.2**: **Mapování OID** (volitelné) – načítání a parsování.
* **M3.3**: **Generování Gauge** metriky (hodnota, OID, časové razítko).
* **M3.4**: **HTTP Klient** pro POST požadavky.
* **M3.5**: **Sestavení/Export** finálního JSON těla na endpoint.
* **M3.6**: **Zpracování chyb HTTP** (ne 2xx kódy), zabránění pádu.

---

## Milník 4: Testování a Dokončení

* **M4.1**: **Verbose režim** (`-v`) pro ladicí výstupy. ✅
* **M4.2**: **Makefile test** cíl (`make test`).
* **M4.3**: **Unit testy** (ASN.1 BER, CLI, OID soubor).
* **M4.4**: **Integrační testy** (falešný SNMP/OTEL, volitelné).
* **M4.5**: **Finální kontrola** (eva/merlin kompatibilita, C++, bez root).