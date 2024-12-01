# osu-ds

As part of an effort to open source the full osu! stack, I (peppy) am splitting out ports which are currently stored in the monolithic git repository. This is a NDS port made by Echo which was developed to a basic working state. Keep in mind that the last changes were made in 2009 and therefore it may not work with newer maps.

No support is provided for this.

# Compiling

You need to install the [devkitPRO](https://devkitpro.org/wiki/Getting_Started) on your machine.
Download or clone this repository in a directory of your choice (let's call it "osu-ds").
Open the "osu-ds" directory and, if you are on windows, simply execute the file `build.bat`. If you are not, simply run `make` on a terminal open "osu-ds" directory.
Wait for the process to finish and if everything goes right, at the end you will have a file called `osuNDS.nds` on the "osu-ds" directory.

# Runing

If you want to run the game on a emulator, you need to enable DLDI on the emulator and set up the direcoty/file from which the files(aka, songs) will be read from.
On the emulator melonDS(0.9.5 or higher), you can do that by going into the menu Config -> Emu Setting -> Choosing the "DLDI" tab -> click on the "Enable DLDI(for homebrew)" checkbox -> click on the "Sync SD to folder" checkbox -> Choose a folder where you will put the songs.(let's call it "songs")
Now load the `osuNDS.nds` on the emulator and the game should start, even if you have no musics to pick from yet.
If you want to run this on original hardware, you will need a flash cart and the DLDI patch for the said cart. Apply the DLDI patch on the `osuNDS.nds` and copy the file to the SD card that will go on the flash cart.
Warning: the latest version of this homebrew has not been tested on any flash cart yet. Any feedback in this process is appreciatted.

# Converting songs

To convert songs from the base Osu! game, you need to compile the content of `format_downgrade` directory first.
It's a C++(17) application, so check if your compiler has support to this version.The make application is also used, so check if this application is installed as well. You also need to have the libraries `mpg123`, `samplerate` and `shlwapi` installed. Those libraries can be installed through pacman on windows.
Once those libraries and a compatible compiler are installed, the application can be compiled by runing the command `make` on the `format_downgrade` directory. This command will generate a file called `format_downgrader.exe` on windows. You can use `format_downgrader.exe` to convert songs to a format that can be played on osu-ds.

To convert a song, you will need two files: the .osu file of the song on the difficulty you want, and the corresponding mp3 music file(.ogg files and others are not supported yet).
Now, simple grab the .osu file and drop over the `format_downgrader.exe` file. This will generate two files: a .ods file and .raw file on the same directory where the .osu file was.
Those two files are compatible with the format that the osuNDS.nds can read.

As a alternative, you can also check this [other converter being created by other contributors of this project](https://github.com/KonPet/osu-ds-convert).

# Adding songs

Remenber the "songs" folder? Create a subdirectory called "osuds" there. Now, put the files .ods and .raw in this subdirectory. If you run the emulator, you should be able to see the song, and by selecting it, you can play it.


## License

This is released under the [BSD 2-Clause license](LICENSE).
