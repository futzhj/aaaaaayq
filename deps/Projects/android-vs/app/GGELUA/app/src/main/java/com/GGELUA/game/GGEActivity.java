package com.GGELUA.game;

import org.libsdl.app.SDLActivity;

/**
* A sample wrapper class that just calls SDLActivity 
*/ 

public class GGEActivity extends SDLActivity
{
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "vgmstream", // Load this first so it's in memory for fsb.c
            "main"       // Then load the main game engine
        };
    }
}
