Overview
========

Wynn is an open source application which aims to facilitate the study of
languages by automating the management of vocabulary wordlists. It has the
functionality of a simple spaced repetition flashcard program in that it allows
the user to create and manage databases of vocabulary and run quizzes which
test the level of assimilation of a particular range of material. As a side
consequence of these quizzes, the database organizes itself, later allowing to
run a quiz on items fulfilling some criteria, e.g. words which were the most
problematic, words which haven't been tested for a long time etc.

An important feature of Wynn is dictionary integration. The user doesn't need
to leave the program to find new words or copy them into the database manually.
The program was originally written as a dictionary fronted and vocabulary
manager for students of Japanese using the method for learning Han characters
outlined by James Heisig in his "Remebering the Kanji" books. Over time,
support for Mandarin was added which was not much work since the user interface
code did not need to change very much. Later, I realized the program could just
as well be made universal and I moved the language support into a plugin
architecture. Existing support for Japanese and Mandarin was migrated to two
separate plugins and an additional plugin for German was created. Wynn is
therefore extensible and can potentially be made to support any language. 

The  Japanese and Mandarin plugins support Hepburn/Pinyin romanization so
installation of East Asian input engines is not necessary.

Wynn is a C++/Qt desktop application, but an Android version is planned in the
near future to bring all that goodness to mobile devices. Some preliminary
support for synchronizing diverging instances of Wynn databases exists, but an
alternative solution may be ultimately required for synchronization between
mobile devices and the desktop application.

The main graphical application is supported by two run-time libraries:
- ixpdict, which contains routines for dictionary handling
- ixplog for debug logging to a text file

The last library, ixptest, is a simple testing framework useful for developing
automated unit tests. It builds to a static library and is not used by the UI
application.

Installation
============

This program uses qmake as its build environment. The projects that make up the
distribution are described by high-level .pro files, which in turn are used by
qmake to generate platform-specific Makefile-s. 

You will need to install qmake and Qt libraries along with development headers.
Wynn was successfully built with both Qt4 and Qt5.

To build Wynn, enter the top directory (where this file is located) and run
qmake to generate the necessary Makefile:

$ qmake PREFIX=/home/ninja build.pro -o Makefile

The PREFIX variable controls the installation prefix. This defaults to
/usr/local if not otherwise specified on the qmake commandline.

Next, just build the whole distribution by running make in the top directory:

$ make

Last, install the program, which will copy the libraries to PREFIX/lib and
binaries to PREFIX/bin:

$ make install

Congratulations, Wynn is ready to run!

Alternatively, you may want to open and build the project from an IDE. The qmake
.pro files define projects and can be opened from Qt Creator. Because Qt is
cross-platform, you can generate Windows binaries that way (or use qmake to
generate Visual Studio solution files). Refer to the qmake documentation for
further information on the subject.

Obtaining help
==============

For now, no documentation exists and no official support channels are available
except for contacting the developer (me) directly. Please send bug reports
(preferably with a reproduction path), feature requests and general rants to
<szczepanek.d@gmail.com>, but bear in mind that I may take a while to respond
and incoherent ramblings/hateful flames may go unanswered entirely. ;)

Contribution
============

If you want to help developing Wynn, then you are very welcome. Feel free to
contact me at <szczepanek.d@gmail.com> so we can work out a model for
collabration.

Most wanted are people with strong C++/Qt skills, as are skilled Android
developers. People passionate about adding support for their favourite language
are also very welcome. I would also appreciate some help in regard with working
out issues related to lazy replication, database synchronization, cloud storage
etc. for the needs of synchronizing Wynn databases across devices.

Some functionality to implement in the upcoming releases would be:
- ability to deactivate specific plugins in settings
- fix column widths of tables (don't always refresh to the stored value, no
  idea why)
- undo button in database manage panel
- find button in dictionary panel to search inside dictionary search results
- facilitate clipboard search
- Android version (starting with just flashcard functionality, dictionaries may
  come later)
- Better support for database synchronization between application instances
- Optimize memory usage

Contributor list:
1. Darek Szczepanek <szczepanek.d@gmail.com>

Credits
=======

Wynn was inspired by Wakan (http://wakan.manga.cz/), a freeware application
that serves as a Japanese/Mandarin dictionary frontend and vocabulary manager.
However, Wakan is close-sourced and Windows-only which made me create Wynn.
Wynn doesn't have nearly as much features as Wakan does, but I think there is
something to be said for simplicity and besides, Wynn is not limited to those
two languages. Anyways, I wish to thank the author of Wakan for inspiration and
his hard work on that awesome piece of software which is indispensable to
students of Japanese and Mandarin.

I also wish to thank the people who did all the hard work of creating
dictionary content and then put the fruit of their labour online for free. The
default Wynn plugins use dictionary content from the following sources:

1. The KANJIDIC2 Han character dictionary, the JMDict Japanese vocabulary
   dictionary and the RADKFILE Han character radical database which are 
   maintained by Jim Breen and (C) by the Electronic Dictionary Research Group
   (http://www.edrdg.org/) 

2. The CC-CEDICT Mandarin Chinese vocabularydictionary, (C) by MDBD
   (http://www.mdbg.net/) 

3. The Beolingus German vocabulary wordlist, (C) by Frank Richter 

4. The Unihan database, (C) by Unicode Inc.

These projects provided the raw dictionary content which is parsed from text
format using the 'dicttool' utility and saved to Wynn-specific binary files for
quick loading. Without the hard work of the people who made those dictionaries,
Wynn wouldn't be doing anything useful, so thanks!

