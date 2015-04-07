openLRSng packet mode
=====================

Usage of openLRSngDL packet mode

FRAME FORMATS (IN):
===================

  0xf0 [bytes] [data...] [[CRChi] [CRClo]]
    User data frame to be sent
      - bytes = 1..48 (or max packet size)
      - if CRC is enabled CRC16 is required to be present

    When packet has been queued for transmission 'ACK' packet is returned

    If CRC is enabled and incorrect 'NACK' packet is returned and data discarded.

FRAME FORMATS (OUT):
====================

  0xf0 [bytes] [data...] [[CRChi] [CRClo]]
    User data frame received
      - bytes = 1..48
      - if CRC is enabled CRC16 is appended

  0xf0 0x00 [localRSSI] [remoteRSSI] [linkQuality]
    Idle frame, periodically returned if 'status packet mode is enabled'

  0xf0 0xff
    ACK frame (returned when packet is queued for tx)

  0xf0 0xfe
    NACK frame (returned on CRC failure)

