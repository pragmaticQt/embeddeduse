
# HOW TO USE QTest in CAN Bus UnitTests

## J1939 frame explanation
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
 
 ## QTest Data Driven Convention
 
  - Write a test data input function ends with  ```_data ```,
  - Each addColumn() defines one column. This process is like to define a table's schema.
  - Each newRow() adds one test data input. This process is like to insert a row to the table.
 
 ```c++
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
```

 - Write a test execution function same name WITHOUT ```_data ```
 - QFETCH is used to fetch ecah row defined in that xxx_data()
 - QCOMPARE(actual, expected) does the verification job.

 ```c++
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
```


NOTE: Code examples from embeddeduse.com

NOTE: code snipt located in BlogPosts\CanComm\test\test_j1939_frames\test_j1939_frames.cpp

 
