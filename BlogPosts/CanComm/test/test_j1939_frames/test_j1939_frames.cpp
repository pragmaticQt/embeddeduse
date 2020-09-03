// Copyright (C) 2019, Burkhard Stubert (DBA Embedded Use)

#include <QByteArray>
#include <QtDebug>
#include <QtTest>

#include "j1939_broadcast_frames.h"
#include "j1939_frame.h"


class TestJ1939Frames : public QObject
{
    Q_OBJECT


private slots:
    // https://i2.wp.com/www.embeddeduse.com/wp-content/uploads/2020/01/j1939-broadcast-frame-1.png?w=1940&ssl=1

    // Data Page (DP)
    // PDU Specific (PS)
    // PDU Format (PF)
    // Parameter Group Number (PGN) = PDU Format (PF) + PDU Specific (PS)
    // Source Address (SA)

    // 31...29 | 28 27 26 | 25 | 24 | 23...16 | 15...8 | 7...0
    //           priority        DP      PF       PS      SA

    // The PGN consists of the PDU Specific (PS) and the PDU Format (PF).
    // The PS is encoded by the bits 8-15 and

    // the PF by the bits 16-23.
    // If PF is in the range from 0 to 239, the frame is a PDU-Format-1 frame or a peer-to-peer frame.
    // If PF is in the range from 240 to 255, the frame is a PDU-Format-2 frame or a broadcast frame.
    // Bit 24 is the Data Page (DP). You use the DP bit to double the number of available PGNs.
    void testPduFormat_data()
    {
        QTest::addColumn<quint16>("pduFormat");
        QTest::addColumn<bool>("isValid");
        QTest::addColumn<quint32>("frameId");

        // 0 to 511 valid
        QTest::newRow("pf = 73") << quint16{73U} << true << 0x00490000U;
        QTest::newRow("pf = 498") << quint16{498U} << true << 0x01f20000U;
        QTest::newRow("pf = 0") << quint16{0U} << true << 0U;
        QTest::newRow("pf = 511") << quint16{511U} << true << 0x01ff0000U;
        // invalid
        QTest::newRow("pf = 512") << quint16{512U} << false << 0U;
        QTest::newRow("pf = 65535") << quint16{65535U} << false << 0U;
    }
    // The combination of PF and PS yields the PGN.
    // The 4096 numbers from 0xf000 to 0xffff are valid broadcast PGNs.
    // The PGN with PF equal to 255 is for proprietary use.
    void testPduFormat()
    {
        QFETCH(quint16, pduFormat);
        QFETCH(bool, isValid);
        QFETCH(quint32, frameId);

        auto frame{J1939Frame{quint8{0U}, pduFormat, quint8{0U}, quint8{0U}, {}}};
        QCOMPARE(frame.isValid(), isValid);
        if (isValid)
        {
            QCOMPARE(frame.pduFormat(), pduFormat);
            QCOMPARE(frame.frameId(), frameId);
        }
    }

    void testPduSpecific_data()
    {
        QTest::addColumn<quint8>("pduSpecific");
        QTest::addColumn<quint32>("frameId");

        QTest::newRow("ps = 73") << quint8{73U} << 0x00004900U;
        QTest::newRow("ps = 241") << quint8{241U} << 0x0000f100U;
        QTest::newRow("ps = 0") << quint8{0U} << 0x00000000U;
        QTest::newRow("ps = 255") << quint8{255U} << 0x0000ff00U;
    }

    void testPduSpecific()
    {
        QFETCH(quint8, pduSpecific);
        QFETCH(quint32, frameId);

        auto frame{J1939Frame{quint8{0U}, quint8{0U}, pduSpecific, quint8{0U}, {}}};
        QCOMPARE(frame.pduSpecific(), pduSpecific);
        QCOMPARE(frame.frameId(), frameId);
        QVERIFY(frame.isValid());
    }

    void testSourceAddress_data()
    {
        QTest::addColumn<quint8>("sourceAddress");
        QTest::addColumn<quint32>("frameId");

        QTest::newRow("sa = 73") << quint8{73U} << 0x00000049U;
        QTest::newRow("sa = 241") << quint8{241U} << 0x000000f1U;
        QTest::newRow("sa = 0") << quint8{0U} << 0x00000000U;
        QTest::newRow("sa = 255") << quint8{255U} << 0x000000ffU;
    }

    void testSourceAddress()
    {
        QFETCH(quint8, sourceAddress);
        QFETCH(quint32, frameId);

        auto frame{J1939Frame{quint8{0U}, quint8{0U}, quint8{0U}, sourceAddress, {}}};
        QCOMPARE(frame.sourceAddress(), sourceAddress);
        QCOMPARE(frame.frameId(), frameId);
        QVERIFY(frame.isValid());
    }

    void testPriority_data()
    {
        QTest::addColumn<quint8>("priority");
        QTest::addColumn<quint32>("frameId");

        QTest::newRow("prio = 3") << quint8{3U} << 0x0C000000U;
        QTest::newRow("prio = 6") << quint8{6U} << 0x18000000U;
        QTest::newRow("prio = 0") << quint8{0U} << 0x00000000U;
        QTest::newRow("prio = 7") << quint8{7U} << 0x1C000000U;
        QTest::newRow("prio = 8") << quint8{7U} << 0x1C000000U;
        QTest::newRow("prio = 255") << quint8{7U} << 0x1C000000U;
    }

    void testPriority()
    {
        QFETCH(quint8, priority);
        QFETCH(quint32, frameId);

        auto frame{J1939Frame{priority, quint8{0U}, quint8{0U}, quint8{0U}, {}}};
        QCOMPARE(frame.priority(), priority);
        QCOMPARE(frame.frameId(), frameId);
        QVERIFY(frame.isValid());
    }

    void testPeerToPeer_data()
    {
        QTest::addColumn<quint16>("pduFormat");
        QTest::addColumn<bool>("isPeerToPeer");

        QTest::newRow("pf = 0") << quint16{0x00U} << true;
        QTest::newRow("pf = 239") << quint16{0xefU} << true;
        QTest::newRow("pf = 240") << quint16{0xf0U} << false;
        QTest::newRow("pf = 255") << quint16{0xffU} << false;
        QTest::newRow("pf = 256") << quint16{0x100U} << true;
        QTest::newRow("pf = 495") << quint16{0x1efU} << true;
        QTest::newRow("pf = 496") << quint16{0x1f0U} << false;
        QTest::newRow("pf = 511") << quint16{0x1ffU} << false;
    }

    void testPeerToPeer()
    {
        QFETCH(quint16, pduFormat);
        QFETCH(bool, isPeerToPeer);

        auto frame{J1939Frame{6U, pduFormat, 0x28U, 0x02U, {}}};
        QCOMPARE(frame.isPeerToPeer(), isPeerToPeer);
    }

    void testProprietaryFrame_data()
    {
        QTest::addColumn<quint16>("pduFormat");
        QTest::addColumn<bool>("isProprietary");

        QTest::newRow("proprietary/peer-to-peer/low") << quint16(0xefU) << true;
        QTest::newRow("standard/peer-to-peer/low") << quint16(0xeaU) << false;

        QTest::newRow("proprietary/peer-to-peer/high") << quint16(0x1efU) << true;
        QTest::newRow("standard/peer-to-peer/high") << quint16(0x1eaU) << false;

        QTest::newRow("proprietary/broadcast/low") << quint16(0xffU) << true;
        QTest::newRow("standard/broadcast/low") << quint16(0xf0U) << false;

        QTest::newRow("proprietary/broadcast/high") << quint16(0x1ffU) << true;
        QTest::newRow("standard/broadcast/high") << quint16(0x195U) << false;
    }

    void testProprietaryFrame()
    {
        QFETCH(quint16, pduFormat);
        QFETCH(bool, isProprietary);

        auto frame{J1939Frame{6U, pduFormat, 42U, 2U, {}}};
        QCOMPARE(frame.isProprietary(), isProprietary);
    }

    void testGroupFunction_data()
    {
        QTest::addColumn<QByteArray>("payload");
        QTest::addColumn<quint8>("groupFunction");

        QTest::newRow("gf = 0x1a") << QByteArray::fromHex("1a0203") << quint8(0x1aU);
        QTest::newRow("gf = 0x01") << QByteArray::fromHex("010203") << quint8(0x01U);
        QTest::newRow("empty payload") << QByteArray{} << quint8(0xffU);
    }

    void testGroupFunction()
    {
        QFETCH(QByteArray, payload);
        QFETCH(quint8, groupFunction);

        auto frame{J1939Frame{6U, 0xefU, 40U, 2U, payload}};
        QCOMPARE(frame.groupFunction(), groupFunction);
    }

    void testSimplePayload()
    {
        auto payload{QByteArray{8, 1}};
        auto frame{J1939Frame{6U, 0xE0U, 0x28U, 0x02U, payload}};
        QCOMPARE(frame.payload(), payload);
    }

    void testEncodePayload()
    {
        auto eec1{EEC1Frame{4U, 10U, 80U, 56U, 5489U, 13U, 3U, 30U}};
        QCOMPARE(eec1.frameId(), 0x0cf00400U);
        QCOMPARE(eec1.payload(), QByteArray::fromHex("a4503871150d031e"));
    }

    // The second payload argument 0x33 (51) cannot be represented by 4 bits. It is truncated to
    // 4 bits, which yields 0x03.
    void testEncodePayloadWithOutOfRangeValues()
    {
        auto eec1{EEC1Frame{4U, 51U, 80U, 56U, 5489U, 13U, 3U, 93U}};
        QCOMPARE(eec1.frameId(), 0x0cf00400U);
        QCOMPARE(eec1.payload(), QByteArray::fromHex("34503871150d035d"));
    }

    void testEncodePayloadWithNegativeValues()
    {
        auto vehicleSpeed{A03VehicleSpeed{-948, 1347, -5439, 4390U}};
        QCOMPARE(vehicleSpeed.frameId(), 0x18FF3203U);
        QCOMPARE(vehicleSpeed.payload(), QByteArray::fromHex("4cfc4305c1ea2611"));
    }

    void testConvertCanToJ1939FrameByConstructor()
    {
        auto canFrame{QCanBusFrame{0x18FF3203U, QByteArray::fromHex("4cfc4305c1ea2611")}};
        auto j1939Frame{J1939Frame{canFrame}};
        QCOMPARE(j1939Frame.sourceAddress(), quint8{3U});
        QCOMPARE(j1939Frame.pduFormat(), quint16{0xffU});
        QCOMPARE(j1939Frame.pduSpecific(), quint16{0x32U});
        QCOMPARE(j1939Frame.priority(), quint8{6U});
        QVERIFY(!j1939Frame.isPeerToPeer());
    }

    void testConvertCanToJ1939FrameByAssignment()
    {
        auto canFrame{QCanBusFrame{0x18FF3203U, QByteArray::fromHex("4cfc4305c1ea2611")}};
        auto j1939Frame{J1939Frame{6U, 0xE0U, 0x28U, 0x02U, {}}};
        j1939Frame = canFrame;
        QCOMPARE(j1939Frame.sourceAddress(), quint8{3U});
        QCOMPARE(j1939Frame.pduFormat(), quint16{0xffU});
        QCOMPARE(j1939Frame.pduSpecific(), quint16{0x32U});
        QCOMPARE(j1939Frame.priority(), quint8{6U});
        QVERIFY(!j1939Frame.isPeerToPeer());
    }

    void testDecodePayload()
    {
        auto vehicleSpeed{J1939Frame{6U, 255U, 50U, 3U, QByteArray::fromHex("4cfc4305c1ea2611")}};
        QCOMPARE(vehicleSpeed.parameterGroupNumber(), 0xff32U);

        auto payload{vehicleSpeed.decode<A03VehicleSpeed::Payload>()};
        QCOMPARE(payload.targetVehicleSpeed, -948);
        QCOMPARE(payload.actualVehicleSpeed, 1347);
        QCOMPARE(payload.targetVehicleSpdRamp, -5439);
        QCOMPARE(payload.engineSpeed_T2, 4390U);
    }
};

QTEST_GUILESS_MAIN(TestJ1939Frames)

#include "test_j1939_frames.moc"
