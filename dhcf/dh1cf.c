/* Code Sample:
*  The following functions calculate the channel index for a given slot number
*  and sequence identifier,
*/

#include <stdio.h>      /* defines printf for tests */
#include <stdint.h>     /* defines uint32_t etc */

extern uint32_t hashword( const uint32_t *k, size_t length, uint32_t  initval);

/*
  GetUnicastChannelIndex returns the channel index for a unicast schedule for the
  current slot number and node address. Uses hashword(x,3,0) % number of channels
    X = Array of 3 32-bit words made up of slot counter (1) and EUI-64 (2)

  slotNumber is the current slot number.
  MACAddr is the EUI-64 of the node for which the unicast schedule is calculated
  nChannels is the number of channels in the active channel list in use
*/

int getUnicastChannelIndex(uint16_t slotNumber, uint8_t MACAddr[8], int nChannels)
{
  uint16_t channelNumber;
  uint32_t x[3];

  x[0] = (uint32_t)slotNumber;
  x[1] = MACAddr[4] << 24 | MACAddr[5] << 16 | MACAddr[6] <<8 | MACAddr[7];
  x[2] = MACAddr[0] << 24 | MACAddr[1] << 16 | MACAddr[2] <<8 | MACAddr[3];
 
  channelNumber = hashword(x,3,0)  % nChannels;
  return(channelNumber);
}

/*
  getBroadcastChannelIndex returns the channel index for a broadcast schedule
  for the current slot number and broadcast schedule identifier.
  Uses hashword(x,3,0) % number of channels
    X = Array of 3 32-bit words made up of slot counter (1)  and broadcast
    identifier (16-bit identifier in the high order 16-bits of the first 32-bit
    word and the other bits set to zero).

  slotNumber is the current slot number.
  MACAddr is the EUI-64 of the node for which the unicast schedule is calculated
  nChannels is the number of channels in the active channel list in use
 */
int getBroadcastChannelIndex(uint16_t slotNumber, uint16_t broadcastSchedID, int nChannels)
{
  uint16_t channelNumber;
  uint32_t x[3];

  x[0] = (uint32_t)slotNumber;
  x[1] = broadcastSchedID  << 16;
  x[2] = 0;

  channelNumber = hashword(x,3,0)  % nChannels;
  return(channelNumber);
}
