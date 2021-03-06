***********
Get Started
***********

This document is intended to help users set up the software environment for the development of audio applications using hardware based on the ESP32 by Espressif. Through a simple example, we would like to illustrate how to use ESP-ADF (Espressif Audio Development Framework).

To make the start with ESP-ADF quicker, Espressif designed :doc:`ESP32 LyraT <get-started-esp32-lyrat>`, a development board intended to build an audio application with the ESP32.

.. figure:: ../../_static/esp32-lyrat-v4.2-side.jpg
    :alt: ESP32 LyraT audio development board
    :figclass: align-center
    :width: 60%

    ESP32 LyraT audio development board

Click the link below to get started with this board.

.. toctree::
    :maxdepth: 1

    Get Started with ESP32 LyraT <get-started-esp32-lyrat>

If you do not have the :doc:`ESP32 LyraT <get-started-esp32-lyrat>` board, you can still use ESP-ADF for the ESP32 based audio applications. This is providing your board has a compatible audio codec chip, or you develop a driver to support communication with your specific audio codec chip.


About ESP-ADF
=============

The ESP-ADF is available as a set of :adf:`components` to extend the functionality already delivered by the `ESP-IDF <https://github.com/espressif/esp-idf>`_ (Espressif IoT Development Framework).

To use ESP-ADF you need set up the ESP-IDF first, and this is described in the next section.


.. _get-started-setup-esp-idf:

Setup ESP-IDF
=============

Configure your PC according to `ESP32 Documentation <http://esp-idf.readthedocs.io/en/latest/?badge=latest>`_. `Windows <http://esp-idf.readthedocs.io/en/latest/get-started/windows-setup.html>`_, `Linux <http://esp-idf.readthedocs.io/en/latest/get-started/linux-setup.html>`_ and `Mac OS <http://esp-idf.readthedocs.io/en/latest/get-started/macos-setup.html>`_ operating systems are supported.

You have a choice to compile and upload code to the ESP32 by command line with `make <http://esp-idf.readthedocs.io/en/latest/get-started/make-project.html>`_ or using `Eclipse IDE <http://esp-idf.readthedocs.io/en/latest/get-started/eclipse-setup.html>`_.

.. note::

    We are using ``~/esp`` directory to install the toolchain, ESP-IDF, ESP-ADF and sample applications. You can use a different directory, but need to adjust respective commands.

To make the installation easier and less prone to errors, use the ``~/esp`` default directory for the installation. Once you get through ESP-IDF setup and move to the ESP-ADF, you will notice that installation of the ESP-ADF follows the similar process. This should make it even easier to get up and running with the ESP-ADF.

If this is your first exposure to the ESP32 and `ESP-IDF <https://github.com/espressif/esp-idf>`_, then it is recommended to get familiar with `hello_world <https://github.com/espressif/esp-idf/tree/master/examples/get-started/hello_world>`_ and `blink <https://github.com/espressif/esp-idf/tree/master/examples/get-started/blink>`_ examples first. Once you can build, upload and run these two examples, then you are ready to proceed to the next section.


.. _get-started-get-esp-adf:

Get ESP-ADF
===========

.. highlight:: bash

Having the ESP-IDF to compile, build and upload application for ESP32, you can now move to installing audio specific API / libraries. They are provided in `ESP-ADF repository <https://github.com/espressif/esp-adf>`_. To get it, open terminal, navigate to the directory to put the ESP-ADF, and clone it using ``git clone`` command::

    cd ~/esp
    git clone --recursive https://github.com/espressif/esp-adf.git

ESP-ADF will be downloaded into ``~/esp/esp-adf``.


.. _get-started-setup-path:

Setup Path to ESP-ADF
=====================

The toolchain programs access ESP-ADF using ``ADF_PATH`` environment variable. This variable should be set up on your PC, otherwise the projects will not build. The process to set it up is analogous to setting up the ``IDF_PATH`` variable, please see instructions in ESP-IDF documentation under `Add IDF_PATH to User Profile <https://esp-idf.readthedocs.io/en/latest/get-started/add-idf_path-to-profile.html>`_.


.. _get-started-start-project:

Start a Project
===============

After initial preparation you are ready to build the first audio application for the ESP32. The process has already been described in ESP-IDF documentation. Now we would like to discuss again the key steps and show how the toolchain is able to access the ESP-ADF :adf:`components` by using the ``ADF_PATH`` variable.

.. note::

    ESP-ADF is based on a specific release of the ESP-IDF. You will see this release cloned with ESP-ADF as a subdirectory, or more specifically as a submodule e.g. ``esp-idf @ ca3faa61`` visible on the GitHub. Just follow this instruction and the build scripts will automatically reach ESP-IDF from the submodule.


To demonstrate how to build an application, we will use :example:`get-started/play_mp3` project from :adf:`examples` directory in the ADF.

Copy :example:`get-started/play_mp3` to ``~/esp`` directory::

    cd ~/esp
    cp -r $ADF_PATH/examples/get-started/play_mp3 .

You can also find a range of example projects under the :adf:`examples` directory in the ESP-ADF repository. These example project directories can be copied in the same way as presented above, to begin your own projects.


.. _get-started-connect-configure:

Connect and Configure
=====================

Connect the audio ESP32 board to the PC, check under what serial port the board is visible and verify if serial communication works as described `ESP-IDF Documentation <https://esp-idf.readthedocs.io/en/latest/get-started/establish-serial-connection.html>`_.

At the terminal window, go to the directory of ``play_mp3`` application and configure it with ``menuconfig`` by selecting the serial port and upload speed::

    cd ~/esp/play_mp3
    make menuconfig

Save the configuration.


.. _get-started-build-flash-monitor:

Build, Flash and Monitor
========================

Now you can build, upload and check the application. Run::

    make flash monitor -j5

This will build the application including ESP-IDF / ESP-ADF components, upload binaries to your ESP32 board and start the monitor.

.. highlight:: none

::

    ...

    I (299) APP: 1. Start audio codec chip
    I (319) APP: 2. Create audio pipeline, add all elements to pipeline, and subscribe pipeline event
    I (319) APP: 2.0. Create event queue to listen to all events from all elements, pipeline
    I (329) APP: 2.1 Create mp3 decoder to decode mp3 file and set custom read callback
    I (339) APP: 2.2 Create i2s stream to write data to codec chip
    I (349) APP: 2.3 Register all elements to audio pipeline
    I (349) APP: 2.4 Link it together [mp3_music_read_cb]-->mp3_decoder-->i2s_stream-->[codec_chip]
    I (359) APP: 3. Start audio_pipeline
    I (389) APP: Status receive from audio_pipeline,pl:0x3ffb338c, cmd:8, status:12
    W (7019) AUDIO_ELEMENT: [mp3] AEL_MSG_CMD_STOP
    W (7109) AUDIO_ELEMENT: [i2s] AEL_MSG_CMD_STOP
    I (7109) APP: Status receive from audio_pipeline,pl:0x3ffb338c, cmd:8, status:14


If there are no issues, besides the above log, you should hear a sound played for about 5 seconds by the speakers or headphones connected to your audio board. Reset the board to hear it again if required.

Now you are ready to try some other :adf:`examples`, or go right to developing your own applications. Check how the :adf:`examples` are made aware of location of the ESP-ADF. Open the :example_file:`get-started/play_mp3/Makefile` and you should see ::


    PROJECT_NAME := play_mp3
    include $(ADF_PATH)/project.mk

The second line contains ``$ADF_PATH`` to point the toolchain to the ESP-ADF. You need similar ``Makefile`` in your own applications developed with the ESP-ADF.


Update ESP-ADF
================

After some time of using ESP-ADF, you may want to update it to take advantage of new features or bug fixes. The simplest way to do so is by deleting existing ``esp-adf`` folder and cloning it again, which is same as when doing initial installation described in sections :ref:`get-started-get-esp-adf`.

Another solution is to update only what has changed. This method is useful if you have a slow connection to the GitHub. To do the update run the following commands::

    cd ~/esp/esp-adf
    git pull
    git submodule update --init --recursive

The ``git pull`` command is fetching and merging changes from ESP-ADF repository on GitHub. Then ``git submodule update --init --recursive`` is updating existing submodules or getting a fresh copy of new ones. On GitHub the submodules are represented as links to other repositories and require this additional command to get them onto your PC.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    get-started-esp32-lyrat
    get-started-esp32-lyrat-v4
