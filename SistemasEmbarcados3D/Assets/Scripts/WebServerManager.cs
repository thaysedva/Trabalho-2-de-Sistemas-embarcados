using System;
using System.Text;
using NativeWebSocket;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class WebServerManager : MonoBehaviour
{
    public Text ConnectButtonText;
    public TMP_InputField IpField;

    private WebSocket webSocket;
    private bool isConnected;

    public LedBehaviour Led;

    public void Update()
    {
        if (isConnected)
        {
            ConnectButtonText.text = "Desconectar";
            IpField.enabled = false;

            #if !UNITY_WEBGL || UNITY_EDITOR
                webSocket.DispatchMessageQueue();
            #endif
        }
        else
        {
            ConnectButtonText.text = "Conectar";
            IpField.enabled = true;
        }
    }

    public async void SendText(string message)
    {
        if (isConnected)
            await webSocket.SendText(message);
    }

    public async void OnButtonClick()
    {
        if (isConnected)
        {
            try
            {
                await webSocket.Close();
            }
            catch (Exception e)
            {
                Debug.Log(e.Message);
            }
        }
        else
        {
            try
            {
                webSocket = new WebSocket($"ws://{IpField.text}/ws");
                webSocket.OnOpen += () =>
                {
                    isConnected = true;
                };

                webSocket.OnError += (e) =>
                {
                    isConnected = false;
                    webSocket.Close();
                };

                webSocket.OnClose += (e) =>
                {
                    isConnected = false;
                };

                webSocket.OnMessage += (bytes) =>
                {
                    if (Encoding.UTF8.GetString(bytes) == "1")
                    {
                        Led.TurnOn();
                    }
                    else
                    {
                        Led.TurnOff();
                    }
                };

                await webSocket.Connect();
            }
            catch (Exception e)
            {
                Debug.Log(e.Message);
            }
        }
    }
}