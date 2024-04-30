// Login.js

import React, {useState} from 'react';


function Login({ isOpen, onClose, isLogined, changeLoginState }) {
   const modalClass = isOpen ? 'login-modal' : 'login-modal-hidden'; // Добавляем класс 'show', если isOpen равен true
  const internalOnClose = () =>{
    onClose();
    changeLoginState();
  };
  return (
    <>
      {isOpen && (
        <div className="login-modal">
          <div className="login-modal-content">
            <span className="close-login-modal" onClick={internalOnClose}>&times;</span>
            <h2>Login</h2>
            {/* Добавьте здесь содержимое вашей формы входа */}
          </div>
        </div>
      )}
    </>
  );
}

export default Login;
