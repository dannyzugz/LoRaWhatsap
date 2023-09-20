const chatForm = document.getElementById('chat-form');
const chatMessages = document.getElementById('chat-messages');

const socket = new WebSocket('ws://' + location.hostname + ':80/ws');
socket.onopen = (oevent) => {
  socket.onmessage =  (event) => {
    const messageincoming = event.data;
    const messageElem = document.createElement('p');
    messageElem.textContent = messageincoming;

    messageElem.classList.add('message');
    messageElem.classList.add('message-incoming');

    chatMessages.appendChild(messageElem);

    // Scroll chat window to the bottom
    chatMessages.scrollTop = chatMessages.scrollHeight;
  };
}

//listener del formulario
chatForm.addEventListener('submit', function (event) {
  event.preventDefault();

  const message = chatForm.elements['message'].value;
  socket.send(message);
  chatForm.elements['message'].value = '';

  const newMessage = document.createElement('p');
  newMessage.textContent = message;

  // Add classes based on message type
  newMessage.classList.add('message');
  newMessage.classList.add('message-outgoing'); // Assuming it's an outgoing message


  chatMessages.appendChild(newMessage);


  // Scroll chat window to the bottom
  chatMessages.scrollTop = chatMessages.scrollHeight;
});