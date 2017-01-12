To make the __node__ perform queries to the __handler__, the following messages are
sent over RS232.  This is implemented in `src/handler.ino`.

1. __node__ sends to __handler__

        ENQ 0x05

1. __handler__ replies with

        ACK 0x06

1. If the query is `CDA`, __node__ sends the following sequence of bytes to __handler__:

        STX 0x02
        C   0x43
        D   0x44
        A   0x41
        ETX 0x03

1. __handler__ will first respond with

        ENQ 0x05

1. __node__ must confirm readiness with

        ACK 0x06

1. __handler__ will now respond with the info. If the info is `ID0001`, then the
   following sequence of bytes will be sent to __node__

        STX 0x02
        I   0x48
        D   0x44
        0   0x30
        0   0x30
        0   0x30
        1   0x31
        ETX 0x03
        ENQ 0x05

1. Note the last byte, it's an `ENQ`. __node__ must reply with

        ACK 0x06

To summarize, these are what we ought to see in the serial receive buffers of __node__ and __handler__:

    __node__ receive buffer   | __handler__ receive buffer
    --------------------------|-----------------------
                              | `0x05`
    `0x06`                    |
                              | `0x02 0x43 0x44 0x41`
                              | `0x03`
    `0x05`                    |
                              | `0x06`
    `0x02 0x48 0x44 0x30`     |
    `0x30 0x30 0x31 0x03`     |
    `0x05`                    |
                              | `0x06`

