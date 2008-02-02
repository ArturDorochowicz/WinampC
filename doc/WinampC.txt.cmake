Winamp Control Plugin   version ${WINAMPC_VERSION_MAJOR}.${WINAMPC_VERSION_MINOR}.${WINAMPC_VERSION_BUILD}

Copyright (C) ${WINAMPC_COPYRIGHT_YEARS_TEXT} Artur Dorochowicz
All Rights Reserved.

TABLE OF CONTENTS
--------------------------------------------------------------------------------
1. Overview
2. Legal stuff
3. List of services
4. Installation
5. Plugin usage
6. Description of services
7. History
8. Contacting the author
--------------------------------------------------------------------------------

1. OVERVIEW
--------------------------------------------------------------------------------
Winamp Control Plugin is a PowePro plugin which gives you the ability to control
Winamp the  way  you like.  Most  of the  things this  plugin does, can  also be
achieved by using directly Winamp API.  But using clearly named services is much
easier then  using something  like  SendMessage(some  numbers, names  etc.)  The
plugin also allows you to control multiple  Winamp instances independently, even
when they are minimized to system tray.

The plugin was tested with the following Winamp versions: 2.80, 2.81
It should work with all 3.6 and newer versions of PowerPro.

Some of the services depend on the  Winamp's version. If so, it is stated in the
service description.

Plugin is  meant to  be used  with Winamp  versions: 1.x  and 2.x.  There is  no
guarantee that it will work with Winamp 3.x or any later version.


2. LEGAL STUFF
--------------------------------------------------------------------------------
This is a  freeware  program. You  may distribute  it  freely, but  you must not
request or  accept any  amount of money  or other  payment for it.  You must not
change this program's contents  in any way. And, by  the way, you use it at your
own risk.  Distribution is fully  allowed as  long as the plugin  (i.e. plugin's
.dll file and  this .txt info  file) remain  unchanged. When  distributing, both
plugin files (.dll and .txt) must be included.

All used names are the property of their respective owners.
Winamp is Copyright (C) by Nullsoft Inc.
PowerPro and plugin template are Copyright (C) by Bruce Switzer.

3. LIST OF SERVICES
--------------------------------------------------------------------------------
add_track_as_bookmark
autoload_preset_dialog
autoload_save_preset_dialog
block_minibrowser
caption
change_directory
clear_plist
close_winamp
configure_visual_plugin
delete_autoload_preset_dialog
delete_preset_dialog
display_elapsed_time
display_remaining_time
end_of_plist
enqueue_file
execute_visual_plugin
file_open_dialog
flush_plist_cache_buffer
forward_5sec
get_bitrate
get_eq_data
get_eq_data63
get_length
get_net_status
get_number_of_channels
get_playback_status
get_plist_selected_path
get_plist_length
get_plist_position
get_plist_position1
get_position
get_position_in_sec
get_repeat
get_samplerate
get_shuffle
get_version
get_version_hex
get_volume
get_volume255
jump_to_file_dialog
jump_to_time
jump_to_time_dialog
load_default_preset
load_preset_dialog
load_preset_from_eq
minimize
min_restore
move_10_tracks_back
next_track
open_about_box
open_file_info_box
open_preferences
open_skin_selector
open_url_dialog
open_visual_plugin_options
open_visual_options
pause_unpause
play_audio_cd
play_any_audio_cd
play_button
play_selected
previous_track
reload_current_skin
repeat_off
repeat_on
restart_winamp
restore
rewind_5sec
save_preset_dialog
save_preset_to_eqf
set_eq_data
set_eq_data63
set_panning
set_panning127
set_plist_position
set_volume
set_volume255
show_edit_bookmarks
shuffle_off
shuffle_on
song_and_number
song_name
start_of_plist
stop
stop_after_current
stop_with_fadeout
toggle_doublesize
toggle_easymove
toggle_eq_window
toggle_main_window
toggle_minibrowser
toggle_on_top
toggle_plist_window
toggle_plist_windowshade
toggle_repeat
toggle_shuffle
toggle_title_scrolling
toggle_windowshade
unblock_minibrowser
update_info
volume_down
volume_up

4. INSTALLATION
--------------------------------------------------------------------------------
In order to  use the  plugin, you  should place  it in  your PowerPro  'plugins'
directory.  If you  don't have  'plugins'  directory,  you  probably  have older
version  of PowerPro.  In  this  case place  plugin  directly  in your  PowerPro
directory. After that you can use its services just like services from any other
plugin. Read PowerPro's help for general information on using plugins.

5. PLUGIN USAGE
--------------------------------------------------------------------------------
All of the services' arguments lists end with these two optional arguments:
        - "response type"
        - "class name"

NOTE:  Beginning from version 1.1.0 the plugin has new arguments' syntax.
       Now "response type" and "class name" arguments are the last and optional.

"Class name"  is Winamp's  window  class name.  Command  will be  issued to this
window. If you specify an empty string ("") default class name (which is "Winamp
v1.x") will be used.  "Class name" argument  enables you to  control many Winamp
windows independently as  long as they use different  class names. Winamp can be
started with  /class=class_name argument  which runs it with  class name of your
choice.

"Response type" allows  you to choose how  the plugin should  react if specified
Winamp window does  not exist i.e. Winamp  is not running at  all or there is no
Winamp window of specified class.
Giving 1 as an argument shows a window with error explanation in case of error.
Giving 2 sets F0 (flag 0) when Winamp window was found or clears F0 if there was
an error.
Giving 3abcde... returns abcde... in case of error.
Everything else given as the  second argument makes  plugin not react in any way
if specified Winamp window wasn't found.

Example service usage might be:
     WinampC.service_name(arg1, arg2, ..., "response type" arg, "class name" arg)
or   WinampC.service_name(arg1, arg2, ..., "response type" arg)
or   WinampC.service_name(arg1, arg2, ...)

When an optional argument is omitted, the plugin uses default value for omitted argument.
Default values are:
        - "" - for "response type"
        - "Winamp v1.x" - for "class name"
If you want to specify "class name" you must give "response type" argument also.

6. DESCRIPTION OF SERVICES
--------------------------------------------------------------------------------
Remember that in every service "response type" and "class name" may be omitted.

add_track_as_bookmark ("response type", "class name")
        Adds current track as bookmark

autoload_preset_dialog ("response type", "class name")
        Opens auto-load presets dialog.
        Doesn't seem to work on Winamp 2.80

autoload_save_preset_dialog ("response type", "class name")
        Opens auto-load save preset.
        Doesn't seem to work on Winamp 2.80

block_minibrowser ("response type", "class name")
        Blocks the Minibrowser from updates.
        Requires Winamp 2.4+

caption ("response type", "class name")
        Gets full Winamp's window caption. This will be something like this 'Number. Song name - Winamp'
        Can work nice as button name or tool tip when Winamp's option 'Scroll song title in the Windows taskbar' is enabled

change_directory ("directory", "response type", "class name")
        Makes Winamp change to the specified directory.

clear_plist ("response type", "class name")
        Cleares Winamp's internal playlist.

close_winamp ("response type", "class name")
        Closes Winamp. (unbelievable, isn't it :)

configure_visual_plugin ("response type", "class name")
        Opens config window of current visualization plugin.

delete_autoload_preset_dialog ("response type", "class name")
        Opens delete an auto load preset dialog.
        Doesn't seem to work on Winamp 2.80

delete_preset_dialog ("response type", "class name")
        Opens delete preset dialog.
        Doesn't seem to work on Winamp 2.80

display_elapsed_time ("response type", "class name")
        Sets time display mode to elapsed.

display_remaining_time ("response type", "class name")
        Sets time display mode to remaining.

end_of_plist ("response type", "class name")
        Plays the last file on the playlist.
        When shuffle is on it is the last played file.

enqueue_file ("file", "response type", "class name")
        Enqueues specified file.

execute_visual_plugin ("response type", "class name")
        Executes current visualization plugin.

file_open_dialog ("response type", "class name")
        Opens 'Open file(s)' dialog.

flush_plist_cache_buffer ("response type", "class name")
        Flushes the playlist cache buffer.

forward_5sec ("response type", "class name")
        Fast forwards 5 seconds.

get_bitrate ("response type", "class name")
        Retrieves bitrate of current song. This is momentary, not average bitrate.
        Requires Winamp 2.05+

get_eq_data ("position", "response type", "class name")
        Queries the status of the EQ.
        The value returned depends on what "position" is set to:
        Value      Meaning
        ------------------
        0-9        The 10 bands of EQ data. (+20db - -20db)
        10         The preamp value. (+20db - -20db)
        11         Enabled. zero if disabled, nonzero if enabled.
        12         Autoload. zero if disabled, nonzero if enabled.

        For positions 0-10 you will get number from between -20.0 and 20.0
        Requires Winamp 2.05+

get_eq_data63 ("position", "response type", "class name")
        Queries the status of the EQ.
        The value returned depends on what "position" is set to:
        Value      Meaning
        ------------------
        0-9        The 10 bands of EQ data. 0-63 (+20db - -20db)
        10         The preamp value. 0-63 (+20db - -20db)
        11         Enabled. zero if disabled, nonzero if enabled.
        12         Autoload. zero if disabled, nonzero if enabled.

        0 is +20db, 63 is -20dB
        Weird, but that's how Winamp handles it internally.
        Requires Winamp 2.05+

get_length ("response type", "class name")
        Returns current song's length in seconds.

get_net_status ("response type", "class name")
        Returns 1 if the Internet connection is available for Winamp.
        Otherwise it returns 0.
        Requires Winamp 2.05+

get_number_of_channels ("response type", "class name")
        Retrieves number of channels of the current song.
        Requires Winamp 2.05+

get_playback_status ("response type", "class name")
        Returns the status of playback.
        Returns 1, if it is playing.
        If it returns 3, it is paused.
        If it returns 0, it is not playing.

get_plist_selected_path ("response type", "class name")
        Gets full path of currently selected playlist entry.
        Requires Winamp 2.04+

get_plist_length ("response type", "class name")
        Gets playlist length in tracks
        Requires Winamp 2.0+

get_plist_position ("response type", "class name")
        Returns the position in the current playlist, in tracks.
        Requires Winamp 2.05+

get_plist_position1 ("response type", "class name")
        Writes the current playlist to <winampdir>\\Winamp.m3u, and returns the current playlist position.
        Requires Winamp 1.666+

get_position ("response type", "class name")
        Returns the position in milliseconds of the current song.

get_position_in_sec ("response type", "class name")
        Returns the position in seconds of the current song.

get_repeat ("response type", "class name")
        Returns the status of the Repeat option (1 if set).
        Requires Winamp 2.4+

get_samplerate ("response type", "class name")
        Retrieves currents songs sample rate
        Requires Winamp 2.05+

get_shuffle ("response type", "class name")
        Returns the status of the Shuffle option (1 if set).
        Requires Winamp 2.4+

get_version ("response type", "class name")
        Retrieves the version of Winamp as x.yz
        Because of the wealth of hexadecimal Winamp version formats (see below: get_version_hex) this service
        may not return correct result. It works by simply leaving three first digits in hexadecimal version
        number and by putting a dot after first of these digits.

get_version_hex ("response type", "class name")
        Retrieves the version of Winamp in hexadecimal format.
        That's what Nullsoft says about that:
                "Version will be 0x20yx for winamp 2.yx. Versions previous to Winamp 2.0
                typically (but not always) use 0x1zyx for 1.zx versions. Weird, I know."

get_volume ("response type", "class name")
        Retrieves volume level as a value from 0 (mute) to 100 (max).

get_volume255 ("response type", "class name")
        Retrieves volume level as a value from 0 (mute) to 255 (max).
        This is the way Winamp is really storing volume.

jump_to_file_dialog ("response type", "class name")
        Opens 'Jump to file' dialog.

jump_to_time ("time", "response type", "class name")
        Jumps to "time" specified in milliseconds.
        Returns -1 if Winamp is not playing, 1 on end of file, or 0 if it was successful.
        Requires Winamp 1.60+

jump_to_time_dialog ("response type", "class name")
        Opens 'Jump to time' dialog.

load_default_preset ("response type", "class name")
        Loads default preset.
        Doesn't seem to work on Winamp 2.80

load_preset_dialog ("response type", "class name")
        Opens load presets dialog.
        Doesn't seem to work on Winamp 2.80

load_preset_from_eq ("response type", "class name")
        Loads a preset from EQ.
        Doesn't seem to work on Winamp 2.80

minimize ("response type", "class name")
        Minimizes Winamp window.

min_restore ("response type", "class name")
        Minimizes Winamp window when not minimized, restores if minimized.

move_10_tracks_back ("response type", "class name")
        Moves 10 tracks back and starts playing.

next_track ("response type", "class name")
        Starts next track.

open_file_info_box ("response type", "class name")
        Opens file info window.

open_about_box ("response type", "class name")
        Opens 'About Winamp' window.

open_preferences ("response type", "class name")
        Opens preferences window.

open_skin_selector ("response type", "class name")
        Opens skin selector window.

open_url_dialog ("response type", "class name")
        Opens 'Open location' dialog.

open_visual_plugin_options ("response type", "class name")
        Opens visualization plug-in options.

open_visual_options ("response type", "class name")
        Opens visualization options.
        Doesn't seem to work on Winamp 2.80

pause_unpause ("response type", "class name")
        Pauses, unpauses. (Why am I writing this? :))

play_audio_cd ("response type", "class name")
        Plays audio CD.

play_any_audio_cd ("drive number", "response type", "class name")
        Plays audio CD in drive specified by "drive number".
        To play from first CD drive "drive number" should be 0, to play from second - give 1 and so on.
        "Drive number" equal to 0 does the same job as play_audio_cd service. 

        This service uses undocumented commands of Winamp API.
        It is not guaranteed to work for everybody, on all versions of Winamp.

play_button ("response type", "class name")
        Same as hitting play button. There's also play_selected service (see below).
        According to Nullsoft there are some differences between them.

play_selected ("response type", "class name")
        Begins play of selected track.

previous_track ("response type", "class name")
        Starts previous track.

reload_current_skin ("response type", "class name")
        Reloads current skin.

repeat_off ("response type", "class name")
        Turns repeat off.
        Requires Winamp 2.4+

repeat_on ("response type", "class name")
        Turns repeat on.
        Requires Winamp 2.4+

restart_winamp ("response type", "class name")
        Restarts Winamp.
        Requires Winamp 2.2+

restore ("response type", "class name")
        Restores Winamp window and makes foreground.
        The window is made foreground even when it is not minimized.

rewind_5sec ("response type", "class name")
        Rewinds 5 seconds.

save_preset_dialog ("response type", "class name")
        Opens save preset dialog.
        Doesn't seem to work on Winamp 2.80

save_preset_to_eqf ("response type", "class name")
        Saves a preset to EQF.
        Doesn't seem to work on Winamp 2.80

set_eq_data ("position", "value", "response type", "class name")
        Sets the value of "position"
        Position      Meaning
        ------------------
        0-9        The 10 bands of EQ data. (+20db - -20db)
        10         The preamp value. (+20db - -20db)
        11         Enabled. zero if disabled, nonzero if enabled.
        12         Autoload. zero if disabled, nonzero if enabled.

        For positions 0-10 you can give any number from between -20.0 and 20.0
        Winamp does not refresh the equalizer window after changing
        positions 0-10, so we can only hear the change.
        Requires Winamp 2.05+

set_eq_data63 ("position", "value", "response type", "class name")
        Sets the value of "position"
        Position      Meaning
        ------------------
        0-9        The 10 bands of EQ data. 0-63 (+20db - -20db)
        10         The preamp value. 0-63 (+20db - -20db)
        11         Enabled. zero if disabled, nonzero if enabled.
        12         Autoload. zero if disabled, nonzero if enabled.

        0 is +20db, 63 is -20dB
        Weird, but that's how Winamp handles it internally.
        Winamp does not refresh the equalizer window after changing
        positions 0-10, so we can only hear the change.
        Requires Winamp 2.05+

set_panning ("panning", "response type", "class name")
        Sets the panning of Winamp (from -100 (left) to 100 (right)).
        Requires Winamp 2.0+

set_panning127 ("panning", "response type", "class name")
        Sets the panning of Winamp (from -127 (left) to 127 (right)).
        This is the way Winamp is really handling panning.
        Requires Winamp 2.0+

set_plist_position ("position", "response type", "class name")
        Sets the playlist position to "position".
        Requires Winamp 2.0+

set_volume ("volume", "response type", "class name")
        Sets volume to approximately "volume" (min = 0, max = 100)
        Requires Winamp 2.0+

set_volume255 ("volume", "response type", "class name")
        Sets volume to "volume" which can be anything between 0 (min) and 255 (max).
        This is the way Winamp is really handling volume.
        Requires Winamp 2.0+

show_edit_bookmarks ("response type", "class name")
        Shows edit bookmarks window.

shuffle_off ("response type", "class name")
        Turns shuffle off.
        Requires Winamp 2.4+

shuffle_on ("response type", "class name")
        Turns shuffle on.
        Requires Winamp 2.4+

song_and_number ("response type", "class name")
        Gets song name with preceding playlist entry number from Winamp's window caption.
        In order for this to work properly Winamp's option 'Scroll song title in the Windows taskbar'
        should be disabled. Otherwise you'll get strange results.

song_name ("response type", "class name")
        Gets song name from Winamp's window caption.
        In order for this to work properly Winamp's option 'Scroll song title in the Windows taskbar'
        should be disabled. Otherwise  you'll get strange results.

start_of_plist ("response type", "class name")
        Plays the first file on the playlist.
        When shuffle is on it is the first played file.

stop ("response type", "class name")
        Stops playback.

stop_after_current ("response type", "class name")
        Makes Winamp stop after current track.

stop_with_fadeout ("response type", "class name")
        Stops with fadeout.

toggle_doublesize ("response type", "class name")
        Toggles double size.

toggle_easymove ("response type", "class name")
        Toggles easy move.

toggle_eq_window ("response type", "class name")
        Toggles equalizer window

toggle_main_window ("response type", "class name")
        Toggles main window visible.

toggle_minibrowser ("response type", "class name")
        Toggles minibrowser window visible.

toggle_on_top ("response type", "class name")
        Toggles always on top.

toggle_plist_window ("response type", "class name")
        Toggles playlist window

toggle_plist_windowshade ("response type", "class name")
        Toggles Windowshade of playlist window.

toggle_repeat ("response type", "class name")
        Toggles repeat.

toggle_shuffle ("response type", "class name")
        Toggles shuffle.

toggle_title_scrolling ("response type", "class name")
        Toggles title scrolling.

toggle_windowshade ("response type", "class name")
        Toggles Windowshade of main window.

unblock_minibrowser ("response type", "class name")
        Unblocks the Minibrowser from updates.
        Requires Winamp 2.4+

update_info ("response type", "class name")
        Asks Winamp to update the informations about the current title.
        Requires Winamp 2.2+

volume_down ("response type", "class name")
        Makes Winamp play a little quiter.

volume_up ("response type", "class name")
        Makes Winamp play a little louder.

7. HISTORY
--------------------------------------------------------------------------------
v. 1.1.2   2007-11-13
   - added get_plist_selected_path

v. 1.1.1
   - added play_any_audio_cd

v. 1.1.0
   - "response type" and "class name" became the last and optional arguments
   - new "response type" defined: 3text... - see description
   - plugin is much smaller now

v. 1.0.2
   - renamed get_version to get_version_hex
   - added new get_version service - see description
   - added minimize, restore, min_restore
   - minor changes in plugin and info file

v. 1.0.1
   - renamed set_panning service to set_panning127
   - added new set_panning service - see description
   - minor fixes in plugin and info file

v. 1.0.0
   - first public release

8. CONTACTING THE AUTHOR
--------------------------------------------------------------------------------
All comments, feature requests, bug reports are welcome and greatly appreciated.

Mail to: Artur Dorochowicz <x-phile@wp.pl> or use PowerPro group.
