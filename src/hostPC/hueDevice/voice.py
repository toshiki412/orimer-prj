import time
import speech_recognition as sr

def controllByVoice(text):
    i = 0
    while i < len(text):
        if   text[i : i + 4] == "ピキピキ":
            print("ひろ君", end = "")
            i += 4
        elif text[i:i+2] == "後ろ":
            print("↓", end="")
            i += 2
        elif text[i] == "右":
            print("→", end="")
            i += 1
        elif text[i] == "左":
            print("←", end="")
            i += 1
        elif text[i] == "前":
            print("↑", end="")
            i += 1
        else:
            i += 1  # 不明な文字はスキップ
    print()  # 改行


def callback(recognizer, audio):
    try:
        print("===========")
        text = recognizer.recognize_google(audio, language="ja-JP")
        print(f"{text}")
        controllByVoice(text)

    except :
        pass

def main():
    rec = sr.Recognizer()
    mic = sr.Microphone()
    stopListening = rec.listen_in_background(mic, callback)
    try:
        while True:
            time.sleep(0.1)
    except KeyboardInterrupt:
        stopListening()

if __name__ == "__main__":
    main()

