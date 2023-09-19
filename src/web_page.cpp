//
// Created by Dani on 9/19/2023.
//
#include <Arduino.h>
#include "web_page.h"

String html = R"====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Chat Interface</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f5f5f5;
        }

        .chat-container {
            width: 400px;
            margin: auto;
            border: 1px solid #ccc;
            padding: 10px;
            padding-bottom: 50px;
        }

        .chat-messages {
            height: 300px;
            overflow-y: auto;
            border: 1px solid #ccc;
            padding: 10px;
            margin-bottom: 10px;
            display: flex;
            flex-direction: column;
        }

        .chat-form {
            display: flex;
        }

        .chat-form input[type="text"] {
            flex-grow: 1;
            border: none;
            border-radius: 20px;
            padding: 10px;
            margin-right: 10px;
        }

        .chat-form button {
            border: none;
            background-color: #007BFF;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 20px;
            margin-left: 10px;
            cursor: pointer;
        }

        .chat-form button:hover {
            background-color: #0056b3;
        }

        .message {
            max-width: 70%;
            margin-bottom: 10px;
            padding: 5px;
            border-radius: 10px;
            color: white;
            line-height: 1.2;
        }

        .message-incoming {
            background-color: #007BFF;
            text-align: left;
            align-self: flex-start;
        }

        .message-outgoing {
            background-color: purple;
            text-align: right;
            align-self: flex-end;
        }

        /* CSS rules for large screens (min-width: 600px) */
        @media screen and (min-width: 600px) {
            .chat-container {
                width: 600px;
                margin: auto;
                border: 1px solid #ccc;
                padding: 20px;
                padding-bottom: 50px;
            }

            .chat-messages {
                height: 500px;
                overflow-y: auto;
                border: 1px solid #ccc;
                padding: 20px;
                margin-bottom: 20px;
            }

            .chat-form input[type="text"] {
                flex-grow: 1;
                border: none;
                border-radius: 20px;
                padding: 15px;
                margin-right: 20px;
            }

            .chat-form button {
                border: none;
                background-color: #007BFF;
                color: white;
                padding: 15px 30px;
                border-radius: 20px;
                cursor: pointer;
            }

            .message {
                max-width: 80%;
                margin-bottom: 15px;
                padding: 10px;
                border-radius: 15px;
                color: white;
                line-height: 1.4;
            }

            .message-incoming {
                background-color: #007BFF;
                text-align: left;
                align-self: flex-start;
            }

            .message-outgoing {
                background-color: purple;
                text-align: right;
                align-self: flex-end;
            }
        }

        /* CSS rules for small screens (max-width: 600px) */
        @media screen and (max-width: 600px) {
            .chat-container {
                width: 100%;
                margin: 0;
                border: none;
                padding: 10px;
            }

            .chat-messages {
                height: 300px;
                overflow-y: auto;
                border: 1px solid #ccc;
                padding: 10px;
                margin-bottom: 10px;
            }

            .chat-form input[type="text"] {
                flex-grow: 1;
                border: none;
                border-radius: 20px;
                padding: 10px;
                margin-right: 10px;
            }

            .chat-form button {
                border: none;
                background-color: #007BFF;
                color: white;
                padding: 10px 20px;
                border-radius: 20px;
                cursor: pointer;
            }

            .message {
                max-width: 70%;
                margin-bottom: 10px;
                padding: 5px;
                border-radius: 10px;
                color: white;
                line-height: 1.2;
            }

            .message-incoming {
                background-color: #007BFF;
                text-align: left;
                align-self: flex-start;
            }

            .message-outgoing {
                background-color: purple;
                text-align: right;
                align-self: flex-end;
            }
        }
    </style>
</head>

<body>
    <div class="chat-container" id="chat-container">
        <div class="chat-messages" id="chat-messages">

        </div>
        <form class="chat-form" id="chat-form">
            <input type="text" id="message" placeholder="Escribe un mensaje..." autocomplete="off">
            <button type="submit">Enviar</button>
        </form>
    </div>
    <script>
        const chatForm = document.getElementById('chat-form');
        const chatMessages = document.getElementById('chat-messages');

        let socket = new WebSocket('ws://' + location.hostname + ':80/ws');


        chatForm.addEventListener('submit', function (event) {
            event.preventDefault();

            const message = chatForm.elements['message'].value;
            socket.send(message);
            chatForm.elements['message'].value = '';

            const newMessage = document.createElement('p');
            newMessage.textContent = message;
            socket.onmessage = function (event) {
                let messagein = event.data;
                let messageElem = document.createElement('p');
                messageElem.textContent = messagein;

                messageElem.classList.add('message');
                messageElem.classList.add('message-incoming');

                chatMessages.appendChild(messageElem);

                // Scroll chat window to the bottom
                chatMessages.scrollTop = chatMessages.scrollHeight;
            };

            // Add classes based on message type
            newMessage.classList.add('message');
            newMessage.classList.add('message-outgoing'); // Assuming it's an outgoing message


            chatMessages.appendChild(newMessage);

            // Scroll chat window to the bottom
            chatMessages.scrollTop = chatMessages.scrollHeight;
        });


    </script>
</body>

</html>
)====";