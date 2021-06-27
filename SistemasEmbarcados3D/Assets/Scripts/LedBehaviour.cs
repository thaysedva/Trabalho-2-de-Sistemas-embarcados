using UnityEngine;

public class LedBehaviour : MonoBehaviour
{
    public Light Light;
    public Material MaterialOn;
    public Material MaterialOff;

    public void TurnOn()
    {
        GetComponent<MeshRenderer>().sharedMaterial = MaterialOn;
        Light.gameObject.SetActive(true);
    }

    public void TurnOff()
    {
        GetComponent<MeshRenderer>().sharedMaterial = MaterialOff;
        Light.gameObject.SetActive(false);
    }
}