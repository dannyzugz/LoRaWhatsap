const chatForm = document.getElementById('chat-form');
const chatMessages = document.getElementById('chat-messages');


chatForm.addEventListener('submit', function(event) {
  event.preventDefault();

  const message = chatForm.elements['message'].value;
  chatForm.elements['message'].value = '';
  const newMessage = document.createElement('p');
  newMessage.textContent = message;

  // Add classes based on message type
  newMessage.classList.add('message');
  newMessage.classList.add('message-outgoing'); // Assuming it's an outgoing message


  chatMessages.appendChild(newMessage);
});
