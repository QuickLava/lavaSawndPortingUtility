# lavaSawndPortingUtility
A program which automates the process of porting sound effects and properties from one character's soundbank to another, and automatically generates patch files for use with programs that edit SFX ID references in Super Smash Bros. Brawl moveset files. 

This program was based directly on work by:
- Jaklub and Agoaj, as well as mstaklo, ssbbtailsfan, stickman and VILE (Sawndz, Super Sawndz)
- Soopercool101, as well as Kryal, BlackJax96, and libertyernie (BrawlLib, BrawlBox, BrawlCrate)

# Using this Program to Port Soundbanks for Use With BrawlEX/P+EX Clones
## Pre-built Soundbanks
If you intend to base your clone's soundbank off of a vanilla Brawl character's, you actually don't need to use this program at all! They've all been ported ahead of time, and can be found [here](https://drive.google.com/drive/folders/1Cb-1fEHXq6LIsUFkfyqixGgCxMRnP_PR).
Simply grab the appropriate .zip for your desire character's soundbank, and follow the instructions below, starting at Step 3.

## Necessary Materials
- A functional BrawlEX/P+EX clone, with a unique Soundbank ID assigned in their "FighterConfigXX.dat". Follow the official [BrawlEX Guide for P+Ex](https://docs.google.com/document/d/1ZoL_qDcwUpUXg82cKaUp-6D_AcfpFctoW6GXFY74_0k/edit#) if you don't have this done yet.

- [The latest release](https://github.com/QuickLava/lavaSawndPortingUtility/releases) of this program.

- The latest release of [lavaSawndIDReplaceAssist](https://github.com/QuickLava/lavaSawndIDReplaceAssist/releases).

- The latest release of [BrawlCrate](https://github.com/soopercool101/BrawlCrate/releases).

- A copy of "smashbros_sound.brsar" from [Soopercool's Brawl File Repository](https://gitlab.com/soopercool202/Super-Smash-Bros-Brawl/-/blob/USA/sound/smashbros_sound.brsar).

- A copy of JOJI's [Empty Snake Soundbank](https://ux.getuploader.com/ssbbhack/download/45).

- Codes' "Porting_Tools.zip", linked near the top of the official [BrawlEX Guide for P+Ex](https://docs.google.com/document/d/1ZoL_qDcwUpUXg82cKaUp-6D_AcfpFctoW6GXFY74_0k/edit#).

- The latest release of [SuperSawndz](http://forums.kc-mm.com/Gallery/BrawlView.php?Number=27683) (or, if that's down, use this [mirror](https://gamebanana.com/tools/6278) on GameBanana).

## Instructions

### Step 1: Inserting JOJI's Blank Snake Soundbank
- Extract the SuperSawndz release .zip, and place your "smashbros_sound.brsar" into the same folder as the included executables.
- Extract JOJI's Empty Snake Soundbank .zip, and place the included "44.sawnd" file into the same folder as your "smashbros_sound.brsar".
- Run "Super Sawndz.exe", and click the "..." button above the "Insert" button, then navigate to and select your "44.sawnd" file.
- In the main Super Sawnd window, press the "Insert" button, and wait for the program to finish.
### Step 1b: Inserting Custom Soundbanks (Optional)
- If you intend to use an existing custom Soundbank for your EX Character, you'll want to insert its ".sawnd" file in the same way that you did the empty "44.sawnd" file.
### Step 2: Running lavaSawndPortingUtility
- Extract the lavaSawndPortingUtility release .zip, and place your "smashbros_sound.brsar" into the same folder as the included executable.
- Open the included "soundbanksToPort.txt" file, and follow its instructions to add a new entry which ports your source character's sounds into a Snake soundbank.
- Run "lavaSawndPortingUtility.exe".
- Collect the output files from the included "Ported Soundbanks" folder, and proceed to the next step.
### Step 3: Running lavaSawndIDReplaceAssist
- Extract the lavaSawndIDReplaceAssist release .zip.
- Run "lavaSawndIDReplaceAssist.exe", select the "Snake->EX" action, then follow the on-screen prompts (type the number associated with the selections you want to make). Be aware: if you're setting up a soundbank for a P+EX clone, add 7 to your clone's Soundbank ID when asked for it.
- Collect the output files from the included "Soundbank_Mapping_Output" folder, and proceed to the next step.
### Step 4: Extracting your Character's "moveset.dat"
- Open your character's moveset file (typically named "Fit[Character].pac", as in "FitMario.pac") in BrawlCrate.
- Right-click the "Misc Data[0]" entry, select "Export", and save the file as "moveset.dat". Proceed to the next step.
### Step 5: Running "sfxchange.exe" (Porting_Tools)
- Extract the "Porting_Tools.zip" file, and copy the "moveset.dat" file you exported in Step 4 into that folder.
- Copy the file you got from Step 2 which ends in "_sound.txt" into this folder, and rename it to "sound.txt".
- Run the "run_sfxchange.bat" file.
- Move your old "moveset.dat" file somewhere else, then rename the newly produced "moveset_sfxconverted.dat" file to "moveset.dat".
- Move your old "sound.txt" file somewhere else, then copy the file you got from Step 3 which ends in "_sound.txt" into this folder. Rename that file to "sound.txt".
- Run the "run_sfxchange.bat" file again.
### Step 6: Replacing your Character's Old "Misc Data[0]"
- Reopen your character's moveset file in BrawlCrate.
- Right-click the "Misc Data[0]" entry, and select "Replace", then navigate to and select your "moveset_sfxconverted.dat" file from Step 5.
- Save the newly edited moveset file.
### Step 7: Placing the New .sawnd and Moveset File onto your SD Card.
- Rename the .sawnd file you got from Step 2 appropriately. For P+EX clones, the name of this file should be the same as the Soundbank ID defined in the clone's "FighterConfigXX.dat". So, for instance, if you've set the Soundbank ID in your character's FighterConfig file to 0x160, your .sawnd file should be named 160.sawnd.
- Copy that file into your build's "/pf/sfx/" directory on your (virtual or non-virtual) SD Card.
- Rename the edited moveset file from Step 6 appropriately, then copy it into your clone's folder in your build's "pf/fighter/" directory.

# Answers to Anticipated Questions and Issues
### In-game Behavior
>My clone is completely silent in-game, what happened?

This is almost certainly a Soundbank ID issue, make sure that you correctly entered your clone's Soundbank ID at every step of this guide. Be especially mindful about whether or not you're providing/looking at decimal or hexadecimal numbers at any given point, as getting the two mixed up can cause problems.

>My clone causes an infinite load when entering/exiting matches, what's going on?

This is most likely because the character's .sawnd file is too big; in my observations, it seems to happen most often past the 950kb mark. To trim it down, I'd suggest either downsampling or entirely removing some of the sounds in your .sawnd; which you can do by importing the file into your "smashbros_sound.brsar" using SuperSawndz, then opening that file in BrawlCrate and editing from there.

>My clone mostly works, but sometimes produces unexpected/incorrect sounds.

This is likely because certain sounds in your source soundbank couldn't be properly mapped to a sound in your destination soundbank, causing every time your moveset calls that sound to instead call a default sound that *did* make it into the resulting bank. Check the log file produced by the program for details on any unusually mapped sounds.

>My clone causes loud static noises during gameplay, what happened?

This is tends to occur when sound data ends up either severely misaligned or otherwise corrupted in the porting process. This usually happens as a result of improperly formatted .sawnd files, though it can *also* be caused by bugs in code. In the event this happens, DM me with the static-y .sawnd and the .sawnd you ported from, and I'll see about fixing it.

### Ported Banks and Log Files

>Can I edit soundbanks produced by this process after the fact?

Absolutely! The banks produced by this program are normal .sawnd files; you can import them into your "smashbros_sound.brsar" using SuperSawndz, then open the .brsar in BrawlCrate and edit sounds as normal.

>How can I verify which sounds were mapped to what in my ported file?

You can find a full list of what's been mapped to what at the bottom of the log file produced by the program. These are found in the same location as the produced soundbank itself, and are suffixed with "_log.txt".

>The produced log file mentions that certain "Wave IDs" are now free to use, what does that mean?

Under the hood, different SFX Entries are mapped to different "Wave Entries", which is what determines what actual sound data gets played when a given SFX ID is called. In some situations, multiple SFX Entries will share a single Wave Entry, usually so that they can play a single sound with different effects (eg. higher or lower volume, different pitch shift, etc).

When SFX Entries which share a Wave Entry in this way are ported over other SFX Entries which *do not* share their Wave Entries with any other sounds, the unique Wave Entries for the overwritten SFX Entries go unused in the final ported soundbank, as no SFX Entries are mapped to them. Despite this, those Wave Entries remain present in the file; and, as a result, can be used to grant unique sound data to SFX Entries which would normally use a shared Wave Entry.

You can do this by changing the SFX Entry's assigned Wave ID in BrawlCrate: open your BRSAR, navigate to the relevant sound, right click it and select "View File", find the relevant sound in the "Data" list in the top-left, then use the "Wave" dropdown in the "Note Info" to select the entry whose number corresponds to the "Orig. Wave ID" listed in the log file. DM me if you'd like more explanation on this.

### Program Behavior

>The "sfxchange.exe" program is exiting without producing a converted file, what gives?

This tends to happen when your files are named incorrectly, so double check that your exported MiscData[0] ("moveset.dat") and "sound.txt" file are named properly. For users whose file explorers hide file extensions, be especially careful that you haven't accidentally named your files something like "moveset.dat.dat" or "sound.txt.txt".

If your files are named correctly but no converted file is being produced, try running "run_sfxlist.bat" instead. If your moveset file is valid and named properly, it should produce a list of all the SFX IDs called in the moveset file. If nothing is produced, double check the file's name; if you can confirm that the name is correct but are still getting nothing, your moveset file may be improperly formed somehow.

If a "soundextract.txt" file is generated by "sfxlist.exe", but "sfxchange.exe" still isn't producing a converted file, it may be that there is an empty line somewhere in your "sound.txt" (the program doesn't like these). Look through the file and remove any empty lines, then save and try running the "sfxchange" file again.

If after doing all the above things still aren't working, DM me and I can see about getting your file converted using an alternative (but WIP) method.

>One of the programs used for this process crashed or otherwise isn't working, what do I do?

If it's one of my programs (usually prefixed by "lava"), you can always DM me on Discord (QuickLava#6688) or Twitter (@QuickLava) for help. If it's not one of mine, you can either ask around the various Brawl Modding discords, or I can still try to help (though I can't guarantee much in the way of solutions in that case). 

### Miscellaneous

>There's a missing step/there's an error/something is unclear in some part of this guide, how can I get it fixed?

Same as the above, DM me and I'll get it sorted as quickly as possible.
