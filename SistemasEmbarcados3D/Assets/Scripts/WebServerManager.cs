using System;
using System.Text;
using NativeWebSocket;
using Newtonsoft.Json.Linq;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class WebServerManager : MonoBehaviour
{
    public Text ConnectButtonText;
    public TMP_InputField IpField;

    private WebSocket webSocket;
    private bool isConnected;

    public LedBehaviour Led1;
    public LedBehaviour Led2;

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
                    var message = Encoding.UTF8.GetString(bytes);

                    var obj = JObject.Parse(message);
                    var messageArray = obj.ToString().Split('{')[1].Split('}')[0].Replace("\"", "").Trim().Split(':');

                    if (messageArray[0].Trim() == "led1")
                    {
                        if (messageArray[1].Trim() == "1")
                            Led1.TurnOn();
                        else
                            Led1.TurnOff();
                    }
                    else if (messageArray[0].Trim() == "led2")
                    {
                        if (messageArray[1].Trim() == "1")
                            Led2.TurnOn();
                        else
                            Led2.TurnOff();
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