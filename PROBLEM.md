# Problem

- [Objective](#objective)
- [Input File](#input-file)
  - [Packet Capture Header](#packet-capture-header)
  - [Packets](#packets)
  - [OUCH Protocol](#ouch-protocol)
    - [System Event Message](#system-event-message)
    - [Accepted Message](#accepted-message)
    - [Replaced Message](#replaced-message)
    - [Executed Message](#executed-message)
    - [Canceled Message](#canceled-message)

## Objective

You are provided with a packet capture file in a proprietary format with OUCH protocol packets strewn across multiple TCP streams. Your goal is to write a program to count the total number of each `message type` across each stream, and for the [Executed Message](#executed-message) sum the `executed shares` field across each stream, and display them as output at the end of the program.

The general format of the output should match something like below:

```
Stream 4
  Accepted: 1330 messages
  System Event: 0 messages
  Replaced: 0 messages
  Canceled: 4129 messages
  Executed: 2 messages: 700 executed shares
...
Stream 3
  Accepted: 8200 messages
  System Event: 0 messages
  Replaced: 0 messages
  Canceled: 8432 messages
  Executed: 4 messages: 346 executed shares

Totals:
  Accepted: 40069 messages
  System Event: 1 messages
  Replaced: 1 messages
  Canceled: 39917 messages
  Executed: 10 messages: 3251 executed shares
```

## Input File

You are provided with the input file [OUCHLMM2.incoming.packets](OUCHLMM2.incoming.packets), consisting of packet capture in a proprietary format. The file itself does not have a header, but consists of records - each representing a packet capture.

### Packet Capture Header

Each packet is framed using the following header:

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned integer | stream idenfitifer |
| 2 | 4 | unsigned integer | packet length (can be 0) |

All integers in the packet capture header are **big endian**.

### Packets

Captured packets contain OUCH protocol message streams. Each packet contains either the full OUCH protocol message, or a partial OUCH protocol message that is completed by a subsequent packet belonging to the same stream.

**Additional clarifications not part of original problem document:**

- Messages can consist of 2 or more partial packets.
- Once a partial packet begins a message for a stream, any subsequent packets for that stream must also be partial packets which continue that same message, until the message is completed.

### OUCH Protocol

The OUCH protocol messages provided in the capture consist of the message types outlined below. All integer fields are **big endian**. Offsets are independent of the [packet capture header](#packet-capture-header).

#### Accepted Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned int | message length, excluding this field (expect 66, 0x42) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'A' (65, 0x41) indicating Accepted** |
| 4 | 8 | unsigned int | timestamp |
| 12 | 14 | text | order token |
| 26 | 1 | char | side |
| 27 | 4 | unsigned int | shares |
| 31 | 8 | text | symbol |
| 39 | 4 | unsigned int | price (x 10,000) |
| 43 | 4 | unsigned int | time in force |
| 47 | 4 | text | firm |
| 51 | 1 | char | display |
| 52 | 8 | unsigned int | order reference number |
| 60 | 1 | char | order capacity |
| 61 | 1 | char | intermarket sweep |
| 62 | 4 | unsigned int | minimum quantity |
| 66 | 1 | char | cross type |
| 67 | 1 | char | order state |

#### Canceled Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned int | message length, excluding this field (expect 29, 0x1D) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'C' (67, 0x43) indicating Canceled** |
| 4 | 8 | unsigned int | timestamp |
| 12 | 14 | text | order token |
| 26 | 4 | unsigned int | decrement shares |
| 30 | 1 | char | reason |

#### Executed Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned int | message length, excluding this field (expect 41, 0x29) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'E' (69, 0x45) indicating Executed** |
| 4 | 8 | unsigned int | timestamp |
| 12 | 14 | text | order token |
| 26 | 4 | unsigned int | executed shares |
| 30 | 4 | unsigned int | executed price (x 10,000) |
| 34 | 1 | char | liquidity flag |
| 35 | 8 | unsigned int | match number |

#### Replaced Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned int | message length, excluding this field (expect 80, 0x50) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'R' (82, 0x52) indicating Replaced** |
| 4 | 8 | unsigned int | timestamp |
| 12 | 14 | text | order token |
| 26 | 1 | char | side |
| 27 | 4 | unsigned int | shares |
| 31 | 8 | text | symbol |
| 39 | 4 | unsigned int | price (x 10,000) |
| 43 | 4 | unsigned int | time in force |
| 47 | 4 | text | firm |
| 51 | 1 | char | display |
| 52 | 8 | unsigned int | order reference number |
| 60 | 1 | char | order capacity |
| 61 | 1 | char | intermarket sweep |
| 62 | 4 | unsigned int | minimum quantity |
| 66 | 1 | char | cross type |
| 67 | 1 | char | order state |
| 68 | 14 | text | previous order token |

#### System Event Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned integer | message length, excluding this field (expect 11, 0x0B) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'S' (83, 0x53) indicating System Event** |
| 4 | 8 | unsigned integer | timestamp |
| 12 | 1 | char | event enumeration |
