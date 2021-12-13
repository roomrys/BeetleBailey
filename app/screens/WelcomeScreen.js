import React from 'react';
import { ImageBackground, StyleSheet, TextInput, View } from 'react-native';

import backgroundImg from "../assets/beetlebailey-comic.png";
import {COLORS} from '../assets/colors.js'

// const image = { backgroundImg };

function WelcomeScreen(props) {
    return (
        <View style={styles.container}>
            <ImageBackground 
            source={backgroundImg}
            style={styles.background}
            >   
                <TextInput
                    style={styles.input}
                    // onChangeText={onChangeNumber}
                    // value={number}
                    maxLength={40}
                    placeholder="max 40char"
                    placeholderTextColor="gray"
                    selectionColor={COLORS.bbred}
                />
            </ImageBackground>
        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
    },
    background: {
        flex: 1,
        justifyContent: "center",
        resizeMode: "contain"
    },
    input: {
        backgroundColor: COLORS.black,
        borderColor: COLORS.bbgreen,
        color: COLORS.bbyellow,
        height: 40,
        margin: 12,
        borderWidth: 5,
        padding: 10,
    }
})

export default WelcomeScreen;