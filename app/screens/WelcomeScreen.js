import React from 'react';
import { ImageBackground, StyleSheet, TextInput, View, SafeAreaView, Pressable, Text } from 'react-native';

import backgroundImg from "../assets/beetlebailey-comic.png";
import {COLORS} from '../assets/colors.js';
import {DIMENS} from '../assets/dimensions.js';

// const image = { backgroundImg };

function WelcomeScreen(props) {
    return (
        <SafeAreaView style={styles.container}>
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
                    placeholder="Insert text to display, then hit 'Display' button"
                    placeholderTextColor="gray"
                    selectionColor={COLORS.bbred}
                />
            <Pressable
                style={styles.button}
                // onPress={onPress}
            >
                <Text style={styles.text}>
                    Display
                </Text>
            </Pressable>
            </ImageBackground>
        </View>
        </SafeAreaView>
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
        ...DIMENS.input,
        backgroundColor: COLORS.black,
        borderColor: COLORS.bbgreen,
        color: COLORS.bbyellow,
    },
    button: {
        ...DIMENS.button,
        backgroundColor: COLORS.bbred,
        alignItems: 'center',
        justifyContent: 'center'

    },
    text: {
        ...DIMENS.text,
        color: COLORS.white
    }
})

export default WelcomeScreen;