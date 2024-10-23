# Hampod Firmware #

## Description ##

This section of the repo is contains all of the code for the Firmware. In the scope of this repo, code is considered Firmware if it controls GPIO pins on the board or if it coordinates with other programs (except Hamlib which is Software controlled). <br>

## Communication ##

The Firmware will idle until a command is sent to it. This is accomplished though the use of named pipes. The two pipes that are used for communication are Firmware_i and Firmware_o. These stand for Firmware in and Firmware out and are from the perspective of the Firmware 
(Firmware_i is the input to the Firmware and Firmware_o is output from the Firmware). <br>
Data is sent through the pipes in a packet formwat which is listed in the following table.

||Type|data length (bytes)|tag|data|
| :---: | :---: | :---: | :---: | :---: |
|Size (bytes)|4|2|2|0-65535|
|Description|The type of packet (keypad, audio, config)|Length of data field in bytes| Tag value for packet. Assigned by the software and can be anything from 0-65535 inclusive.| Additional data from the packet. In practice it is mostly strings but any data can go here.|

## Structure ##
The firmware is broken into three seperate processes: the Firmware controller, keypad code, and audio code. Each process comprises two threads, one that encodes/decodes packets and one that performs actions based off of the decoded packet. The Firmware was designed this way to minimize the amount of blocking. <br>
Every packet sent to the Firmware will get a response packet back from the Firmware except during startup.

### Firmware controller ###
This is the code that the software interacts with. The controller will decode packets to figure out which process the packet is for, if any. Packets that are designated as `config` will change the related instruction in the controller code. Invalid packets are discarded and not forwarded to anything.

### Keypad ###
This code reads the keypad and sends back the read key. The value for a read key is `0-15` inclusive. If no key is pressed or multiple keys are pressed, then the keypad code will return `-1`. The following table corresponds to which value maps to the keys. The numbers in codeblocks (`This format`) correspond to the 
value the code returns.

||Column 1| Column 2| Column 3| Column 4|
|:-:|:-:|:-:|:-:|:-:|
|Row 1|1: `0`|2: `1`|3: `2`|A: `3`|
|Row 2|4: `4`|5: `5`|6: `6`|B: `7`|
|Row 3|7: `8`|8: `9`|9: `10`|C: `11`|
|Row 4|*: `12`|0: `13`|#: `14`|D: `15`|

A properly formatted packet will conatin a lowercase `r` in the data field. More data could follow but the keypad code only cares about the first character in the data field. Subsequent data after the first charater are ignored.

### Audio ###
This code is responsible for audio playback on the Hampod. This code takes string data from a packet and can either utilize text-to speech for the string or play the file specified with the path provided in the packet. <br>
The data field of a audio packet can be broken down into two parts: a character for which type of audio it is and a null-terminated string that is either a valid filepath to a `.wav` file or a string of text. To specify using speech synthesis, the first character should be a lowercase `s` followed by the string
There is no need to put space between the specifier character and the string. For example, the input `sHello World` will send `Hello World` to the speech synthesizer without the first `s`. <br>
To playback an existing `.wav` file, a lowercase `p` should prefix the null-termineated filepath. For example, the data field for a playback packet would look like `ppath/to/file/HelloWorld` and the firmware will play `path/to/file/HelloWorld.wav`. The Firmware will also add the `.wav` to the end of the file, so it
is not necessary to include it in the path. <br>
*Note: The Firmware will attempt to play that file, whether it exists or not. It will return an error if it cannot find the file.*

## Packets Returned ##
During operation outside of startup, the Firmware will always send a response packet back. However, there is no guarantee in the order they are returned. Not every packet will take the same amount of time to be processed. For example, if a text-to speech packet is sent followed by a read keypad packet, then the read keypad packet will be returned first since reading the keypad is a near instant process. The text-to-speech synthesis is much slower, so its packet will be returned after the keypad packet. <br>
This is what the `tag` field is used for. While the Firmware does not use the `tag` field, software can assign each packet it sends to the Frimware with an id and use the ids it sends to determine what response packet was sent back, since it may not be obvious from the `type` field alone.

## Building & Running ##
To build the Firmware, run `make` in this directory to generate the `elf` file. Run `make debug` to get the debug version of the Firmware, which has debug print statements as well as including the debugging symbols for the code. <br>
Each thread and process has a unique text output color for debug print statements. They also have a prefix to let the person debugging know which process/thead it came from (ex. `Audio - Main:` for the main process for the audio section of Frimware).
