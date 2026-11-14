# ncxmms2

[![Build Status](https://travis-ci.org/pkunavin/ncxmms2.svg?branch=master)](https://travis-ci.org/pkunavin/ncxmms2)

## About
ncxmms2 is an ncurses xmms2 client. This project is inspired by
[ncmpcpp](http://ncmpcpp.rybczak.net/) and aimed to have similiar look and feel.

## Status
The project is still under development but already in pretty usable state.
Currently supported features:
 * Base features:
   * Display playback status
   * Playback control
 
  
 * Screens:
  * Help
  * Active playlist
  * Local file system browser
  * Server side browser
  * Medialib browser
  * Playlists browser
  * Equalizer
  * Visualizer (audio spectrum)

## Key bindings
Screens switching:
 * `1` - Help 
 * `2` - Active playlist
 * `3` - Local file system browser
 * `4` - Server side browser
 * `5` - Medialib browser
 * `6` - Playlists browser
 * `7` - Equalizer
 * `8` - Visualizer
  
Playback control:
 * `P` - Toggle
 * `s` - Stop
 * `>` - Next
 * `<` - Prev
 * `f` - Seek forward
 * `b` - Seek backward

List view:
 * `up arrow` - Move cursor up
 * `down arrow` - Move cursor down
 * `Page Up` - Move cursor up by half page
 * `Page Down` - Move cursor down by half page
 * `Home` - Move cursor to the first item
 * `End` - Move cursor to the last item
 * `insert` - Toggle selection
 * `*` - Invert selection
 * `+` - Select items by regular expression
 * `\` - Unselect items by regular expression
 * `.` - Jump to next selected item
 * `,` - Jump to previous selected item
  
Playlists browser:
 * `n` - Create new playlist
 * `Delete` - Remove playlist
 * `r` - Rename playlist
 * `o` - Go to currently active playlist
  
Playlist view:
 * `c` - Clear playlist
 * `S` - Shuffle playlist
 * `o` - Go to currently playing song
 * `m` - Move selected songs
 * `Ctrl + O` - Add file / directory
 * `Ctrl + U` - Add url
 * `i` - Show song info
  
File system browser:
 * `space` - Add file or directory to active playlist
 * `enter` - Play file in active playlist
 * `left arrow` - Go up
 * `g` - Change directory
 * `R` - Reload directory
 * `i` - Show song info
 * ``` - Change tag used for left column
 
Medialib browser:
 * `R` - Refresh
 * `space` - Add artist / album / song to active playlist
 * `enter` - Play artist / album / song in active playlist
 * `i` - Show song info

Visualizer:
 * The spectrum is displayed as 16 bands per channel: the left channel is drawn
   in the left half of the screen and the right channel in the right half.
 * The spectrum is rendered from PCM data provided by the XMMS2 visualization
   subsystem. The `visualization` effect has to be enabled in the XMMS2 effect
   chain, for example (use the first free `effect.order.N` slot):
   ```
   xmms2 server config effect.order.1 visualization
   ```
   The effect is applied when a playback stream is created, so restart playback
   after enabling it. The visualizer screen also shows a hint if it receives no
   data.
 * Refresh rate can be changed with the `fps` option in the `[VisualizerScreen]`
   config section.

## Settings
For customization see ncxmms2.conf and ncxmms2.colors.
