using UnityEngine;
using System.Collections;

public class PacketScript : MonoBehaviour {

	Transform transf_Packet;
	Transform transf_DropPacket;
	//Vector3 src_address;
	Vector3 mid_port;
	Vector3 dst_address;
	Vector3 dropmid_port;
	const float PACKET_LIFE_TIME = 2;
	float packetLifeTimer;
	GameObject pdata;
	// Use this for initialization
	//void Start () {
	
	//}

	public void Init(Vector3 srcPos, Vector3 midPos, Vector3 dstPos, string protocol, int type, GameObject port){
		transf_Packet = GetComponent<Transform>();

		transf_Packet.position = srcPos;
		mid_port = midPos;
		dst_address = dstPos;
		pdata = port;
		Color color = this.gameObject.GetComponent<Renderer>().material.color;

		packetLifeTimer = PACKET_LIFE_TIME;
		switch(protocol){
			case "TCPSYN":
				color = new Color(255,105,180);
				break;
			case "TCPSYNACK":
				color = new Color(255,183,76);
				break;
			case "TCPACK":
				color = new Color(0,0,255);
				break;
			case "TCPRST":
				color = new Color(255,0,0);
				break;
			case "TCPFIN":
				color = new Color(255,0,255);
				break;
			case "UDP":
				color = new Color(0,255,0);
				break;
			case "ICMP":
				color = new Color(255,255,0);
				break;
			default:
				color = new Color(255,255,255);
				break;
		}
		if(type != 0){
			color = new Color(255,0,0);
		}

		gameObject.SetActive(true);

		iTween.MoveTo(
			this.gameObject, 
			iTween.Hash(
			"position", mid_port,
			"time",  1,
			"oncomplete", "onComplete",
			"oncompleteparams", type,
			"oncompletetarget", this.gameObject
			)
		);
	}
	public void onComplete(int type){
		GameObject child = pdata.transform.FindChild("black").gameObject;
		if(type == 0){
			child.gameObject.GetComponent<Renderer>().material.color = Color.blue;			
			//Debug.Log(child.gameObject.GetComponent<Renderer>().material.color);
		}else{
			Debug.Log("drop!");
		}
		iTween.MoveTo(this.gameObject, dst_address, 1);
	}
	// Update is called once per frame
	void Update () {
		packetLifeTimer -= Time.deltaTime;
		if(packetLifeTimer < 0){
			this.gameObject.SetActive(false);
			this.gameObject.GetComponent<Renderer>().material.color = new Color(255,255,255);
		}
	}
}