using UnityEngine;
using System.Collections;

public class PortScript : MonoBehaviour {

	const float PORT_LIFE_TIME = 10;
	float portLifeTimer;

	// Use this for initialization
	//void Start () {
	
	//}
	
	// Update is called once per frame
	public void Init(string port){
		portLifeTimer = PORT_LIFE_TIME;
		this.gameObject.name = port;
		this.gameObject.GetComponent<TextMesh> ().text = port;
		this.gameObject.SetActive(true);
	}
	void Update () {
		portLifeTimer -= Time.deltaTime;
		if(portLifeTimer < 0) {
		this.gameObject.name = "nl_port(Clone)";
		this.gameObject.SetActive(false);
		}
	}
}
