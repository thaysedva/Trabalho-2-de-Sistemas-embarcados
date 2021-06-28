using UnityEngine;

public class ButtonBehaviour : MonoBehaviour
{
    public WebServerManager WebServerManager;
    public bool on;
    public string Command;

    public void Update()
    {
        if (on)
        {
            var button = transform.GetChild(1);
            button.transform.localPosition = new Vector3(0, -0.1f, 0);
        }
        else
        {
            var button = transform.GetChild(1);
            button.transform.localPosition = new Vector3(0, 0.1f, 0);
        }
    }

    public void OnMouseDown()
    {
        on = true;
        WebServerManager.SendText(Command);
    }

    public void OnMouseUp()
    {
        on = false;
    }
}