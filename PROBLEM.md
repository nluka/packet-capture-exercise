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

You are provided with the input file [OUCHLMM2.incoming.packets](/OUCHLMM2.incoming.packets), consisting of packet capture in a proprietary format. The file itself does not have a header, but consists of records - each representing a packet capture.

### Packet Capture Header

Each packet is framed using the following header:

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | Unsigned integer | Stream idenfitifer |
| 2 | 4 | Unsigned integer | Packet length |

All integers in the packet capture header are **Big Endian**.

### Packets

Captured packets contain OUCH protocol message streams. Each packet contains either the full OUCH protocol message, or a partial OUCH protocol message that is completed by a subsequent packet belonging to the same stream.

### OUCH Protocol

The OUCH protocol messages provided in the capture consist of the message types outlined below. All integer fields are **Big Endian**. Offsets are independent of the [packet capture header](#packet-capture-header).

#### System Event Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned integer | message length, excluding this field (expect 11) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'S' indicating System Event** |
| 4 | 8 | unsigned integer | timestamp |
| 12 | 1 | char | event enumeration |

#### Accepted Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned int | message length, excluding this field (expect 66) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'A' indicating Accepted** |
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

#### Replaced Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned int | message length, excluding this field (expect 80) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'R' indicating Replaced** |
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

#### Executed Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned int | message length, excluding this field (expect 80) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'E' indicating Executed** |
| 4 | 8 | unsigned int | timestamp |
| 12 | 14 | text | order token |
| 26 | 4 | unsigned int | executed shares |
| 30 | 4 | unsigned int | executed price (x 10,000) |
| 34 | 1 | char | liquidity flag |
| 35 | 8 | unsigned int | match number |

#### Canceled Message

| Offset in bytes | Size in bytes | Type | Description |
| --------------- | ------------- | ---- | ----------- |
| 0 | 2 | unsigned int | message length, excluding this field (expect 80) |
| 2 | 1 | char | packet type - always 'S' indicating Sequenced |
| 3 | 1 | char | **message type - always 'C' indicating Canceled** |
| 4 | 8 | unsigned int | timestamp |
| 12 | 14 | text | order token |
| 26 | 4 | unsigned int | decrement shares |
| 30 | 1 | char | reason |
