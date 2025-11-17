Adam Žluva
xzluva01

Datum vytvoření: 17. 11. 2025

ISA Projekt 2025: Exportér SNMP Gauge metrik do OpenTelemetry (OTEL)

Tento projekt implementuje program, napsaný v C++, snmp2otel. Program dostane na vstupu adresu zařízení, jehož veličiny chceme sledovat
pomocí protokolu SNMP, cestu k textovému souboru, který obsahuje OID sledovaných veličin, a URL OTEL endpointu, na který se budou posbírané 
veličiny exportovat.

Detaily k implementaci v manual.pdf

Makefile cíle:
    - clean: smaže zkompilovanou binárku a .o soubory
    - all: zkompiluje binárku snmp2otel
    - run: zkompiluje a spustí binárku s těmito parametry: '-t 127.0.0.1 -o data/oids.txt -e http://localhost:4318/v1/metrics -p 1161 -i 3 -v'
    - test: spustí automatické testy (spustilo, kdyby nějaké implementované byly...)

Poznámky ke spuštění:
    - Pokud je parametr nevalidní a jedná se o volitelný, použije se defaultní hodnota. Program se nepřeruší

Příklad spuštění: ./snmp2otel -t 127.0.0.1 -o data/oids.txt -e http://localhost:4318/v1/metrics -p 1161 -i 3 -v

Poznámky k projektu:
    - testy nebyly implementovány
    - do OTEL se nereportují jednotky metriky

K implementaci byla použita knihovna nlohmann/json dostupná z https://github.com/nlohmann/json

Seznam odevzdaných souborů:
./include/
    arguments.hpp
    ber.hpp
    context.hpp
    encoder.hpp
    decoder.hpp
    json.hpp
    oidreader.hpp
    otelexporter.hpp
    snmphelper.hpp
    tags.hpp
    tcpclient.hpp
    udpclient.hpp
    utils.hpp
./src/
    arguments.cpp
    ber.cpp
    decoder.cpp
    encoder.cpp
    main.cpp
    oidreader.cpp
    otelexporter.cpp
    snmphelper.cpp
    tcpclient.cpp
    udpclient.cpp