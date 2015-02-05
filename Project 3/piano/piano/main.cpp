//
//  piano.cpp
//
//  Created by Nilay Modi on 11/2/14.
//  Copyright (c) 2014 Nilay Modi. All rights reserved.
//

#include <iostream>
#include <cctype>
#include <string>
using namespace std;

bool isBetweenAandG(char c){
    if(c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F'  || c == 'G'){
       return true;
    }else{
       return false;
    }
}

bool isTuneWellFormed(string tune)
{
    //if last charachter is not a '/' return false
    //if(tune[tune.size()-1] != '/') return false;
    
    // hold refers to extra stuff held by a single note like octave, accidental sign etc.
    int hold = 0;
    
    //moves i to the next new note after each loop until the end is reached
    for(int i = 0; i != tune.size(); i = i + (++hold)){
        
        if(isBetweenAandG(tune[i])){
            hold = 0;
            if(tune[i+1] != '/' && !isBetweenAandG(tune[i+1])){
                
                // first possibility of #, b or digit
                if(tune[i+1] == '#' || tune[i+1] == 'b' || isdigit(tune[i+1])){
                    hold = 1;
                }else{
                    return false;
                }
                
                // second possibility of just digit & where the previous one is either # or b
                if(hold == 1 && (tune[i+1] == '#' || tune[i+1] == 'b')){
                    if(tune[i+2] != '/' && !isBetweenAandG(tune[i+2])){
                        if(isdigit(tune[i+2])){
                            hold = 2;
                        }else{
                            return false;
                        }
                    }
                }
            }
            
        }else if(tune[i] == '/'){
            hold = 0;
        }else{
            return false;
        }
        
    }
    
    return true;
}

//takes a note in the format (octave, noteLetter, accidentalSign) and translates it
char translateNote(int octave, char noteLetter, char accidentalSign)
{
    // This check is here solely to report a common CS 31 student error.
    if (octave > 9)
    {
        cerr << "********** translateNote was called with first argument = "
        << octave << endl;
    }
    
    // Convert Cb, C, C#/Db, D, D#/Eb, ..., B, B#
    //      to -1, 0,   1,   2,   3, ...,  11, 12
    
    int note;
    switch (noteLetter)
    {
        case 'C':  note =  0; break;
        case 'D':  note =  2; break;
        case 'E':  note =  4; break;
        case 'F':  note =  5; break;
        case 'G':  note =  7; break;
        case 'A':  note =  9; break;
        case 'B':  note = 11; break;
        default:   return ' ';
    }
    switch (accidentalSign)
    {
        case '#':  note++; break;
        case 'b':  note--; break;
        case ' ':  break;
        default:   return ' ';
    }
    
    // Convert ..., A#1, B1, C2, C#2, D2, ... to
    //         ..., -2,  -1, 0,   1,  2, ...
    
    int sequenceNumber = 12 * (octave - 2) + note;
    
    string keymap = "Z1X2CV3B4N5M,6.7/A8S9D0FG!H@JK#L$Q%WE^R&TY*U(I)OP";
    if (sequenceNumber < 0  ||  sequenceNumber >= keymap.size())
        return ' ';
    return keymap[sequenceNumber];
}

//takes a key string like "C#3" or "Bb5" and converts it to a translated note
char oldKeyToTranslatedNote(string key){
    
    int octave = 4;
    char noteLetter = ' ';
    char accidentalSign = ' ';
    
    if(key.size() == 1){
        noteLetter = key[0];
    }else if(key.size() == 2){
        if(key[1] == '#' || key[1] == 'b') accidentalSign = key[1];
        else if(isdigit(key[1])) octave = key[1]-'0';
        noteLetter = key[0];
    }else if(key.size() == 3){
        if(key[1] == '#' || key[1] == 'b') accidentalSign = key[1];
        octave = key[2]-'0';
        noteLetter = key[0];
    }

    return translateNote(octave, noteLetter, accidentalSign);
}

int translateTune(string tune, string& instructions, int& badBeat)
{
    if(!isTuneWellFormed(tune)) return 1; // tune is not wellformed
    else{ // tune is well formed
        string inst = "";
        int currentBeat = 0;
        
        //moves i to the beginning key of the next beat until end is reached
        for(int i = 0; i != tune.size(); i++){
            
            string beat = "";
            
            // checking for soundless beat
            if(tune[i] == '/' && tune[i+1] == '/') beat = " ";
            
            // if beat exists, get the beat
            while(tune[i] != '/'){
                beat =  beat + tune[i];
                i++;
            }
            
            currentBeat++;
            
            // checks the number of keys per beat 0, 1 or >1
            int numberOfKeys = 0;
            for(int j = 0; j != beat.size(); j++){
                if(numberOfKeys >= 2) break; //eliminates unnecessary looping
                if(isBetweenAandG(beat[j])) numberOfKeys++;
            }
            
            if(numberOfKeys == 1){
 
                //in this case beat is the same as key
                char convertedNote = oldKeyToTranslatedNote(beat);
                
                // checks if note/beat is playable
                if(convertedNote != ' '){
                    inst = inst + convertedNote;
                }else{
                    badBeat = currentBeat;
                    return 2;
                }
                
            }else if(numberOfKeys > 1){ // chord
                string key = ""; // key refers to the unconverted note like 'C#5'
                string chord = "[";
                
                //goes through the code and picks up a key after a key
                for(int k = 0; k < beat.size(); k++){
                    key = beat[k];
                    while(!isBetweenAandG(beat[k+1]) && k < beat.size()-1){
                        key = key + beat[k+1];
                        k++;
                    }
                    
                    char convertedNote = oldKeyToTranslatedNote(key);
                    
                    // checks if note is playable
                    if(convertedNote != ' '){
                        chord = chord + convertedNote;
                    }else{
                        badBeat = currentBeat;
                        return 2;
                    }
                }
                //All notes within the chord are playable
                chord = chord + "]";
                inst = inst + chord;
                
            }else{ // empty beat
                inst = inst + " ";
            }
            
        }
        
        //all notes are playabe within the tune
        instructions = inst;
        return 0;
    }
}

int main() {
    
    cout << "Enter Tune : ";
    string tune;
    getline(cin, tune);
    
    string instructions = "";
    int badBeat = 0;
    int k = translateTune(tune, instructions, badBeat);
    
    cout << "'" << k << "'";
    
}


