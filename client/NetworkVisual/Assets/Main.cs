using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

using System;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text.RegularExpressions;
using SpicyPixel.Threading;
using SpicyPixel.Threading.Tasks;

public class UdpState{
	public UdpClient u;
	public IPEndPoint e;
}


public class Main : ConcurrentBehaviour {
	private Thread thread1, thread2, thread3, thread4, thread5;

	public bool pcap_Received = false, add_Received = false, port_Received = false, fw_Received = false, rule_Received = false;
	string pcap_data, add_data, port_data, fw_data, rule_data;
	//Regex regex = new System.Text.RegularExpressions.Regex("[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3},[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3},[0-9]{1,4},[0-9]{1,4}");
  	IPEndPoint pcap_e, add_e, port_e, fw_e, rule_e;
  	UdpClient pcap_u, add_u, port_u, fw_u, rule_u;
  	UdpState pcap_s, add_s, port_s, fw_s, rule_s;

  	string hostadd = "192.168.33.10";

  	//パケットオブジェクトの設定
  	public GameObject pf_Packet;
  	public GameObject pf_Port;
  	List<PacketScript> list_Packets = new List<PacketScript>();
  	const int MAX_PACKETS = 100;
  	List<PortScript> list_ports = new List<PortScript>();
  	const int MAX_PORTS = 100;

  	int createaddcount = 0;

  	//iptableルール設定
  	public GameObject rule_text;
  	public GameObject rule_back;


	// Use this for initialization
	void Start () {
		rule_back = GameObject.Find("rule_back");
		rule_text = GameObject.Find("rule_text");
		rule_back.SetActive(false);
		rule_text.SetActive(false);
		
		thread1 = new Thread(Pcap);
		thread2 = new Thread(Address);
		thread3 = new Thread(Port);
		thread4 = new Thread(Firewall);
		thread5 = new Thread(Rulemsg);

		pcap_e = new IPEndPoint(IPAddress.Any, 12345);
		add_e = new IPEndPoint(IPAddress.Any, 12346);
		port_e = new IPEndPoint(IPAddress.Any, 12347);
		fw_e = new IPEndPoint(IPAddress.Any, 12348);
		rule_e = new IPEndPoint(IPAddress.Any, 12349);

		pcap_u = new UdpClient(pcap_e);
		add_u = new UdpClient(add_e);
		port_u = new UdpClient(port_e);
		fw_u = new UdpClient(fw_e);
		rule_u = new UdpClient(rule_e);

		pcap_s = add_s = port_s = fw_s = rule_s = new UdpState();

		pcap_s.e = pcap_e;
		add_s.e = add_e;
		port_s.e = port_e;
		fw_s.e = fw_e;
		rule_s.e = rule_e;

		pcap_s.u = pcap_u;
		add_s.u = add_u;
		port_s.u = port_u;
		fw_s.u = fw_u;
		rule_s.u = rule_u;

		PacketScript packet;
		for(int i = 0; i < MAX_PACKETS; i++){
			packet = ((GameObject) Instantiate(pf_Packet)).GetComponent<PacketScript>();
			packet.gameObject.SetActive(false);
			list_Packets.Add(packet);
		}
		PortScript port;
		for(int i = 0; i< MAX_PORTS; i++){
			port = ((GameObject) Instantiate(pf_Port, new Vector3(-6 + i%5*6, (-5 - i/5) * 3, 0), Quaternion.identity)).GetComponent<PortScript>();
			port.gameObject.SetActive(false);
			list_ports.Add(port);
		}

		thread1.Start();
		thread2.Start();
		thread3.Start();
		thread4.Start();
		thread5.Start();
	}

	void Update (){
		rule_text.GetComponent<TextMesh>().text = rule_data;
		this.transform.Rotate ( 0, ( Input.GetAxis ( "Horizontal" ) * 1 ), 0 );
	}

	public void Movepacket(int type){
		GameObject obj1, obj2, obj3;
		obj1 = obj2 = obj3 = null;
		Vector3 srcPos, midPos, dstPos;
		string protocol = null;
		string port = null;
		if(type == 0){
			string[] head = pcap_data.Split(',');
			if(head[2] == "0" || head[3] == "0"){
				return;
			}
			if(GameObject.Find(head[1])){
				obj1 = GameObject.Find(head[1]);
			}else{
				obj1 = GameObject.Find("other");
			}
			protocol = head[4];
			if(head[1] == hostadd){
				port = head[3];
			}else{
				port = head[2];
			}
			if(head[4] == "ICMP"){
				obj2 = GameObject.Find("ICMP");
			}else if(GameObject.Find(port)){
				obj2 = GameObject.Find(port);
			}else{
				//ノットリッスンポート作成
				//事前にポートを作っておいて作成しておく
				for(int i = 0; i < list_ports.Count; i++){
					if(list_ports[i].gameObject.activeSelf == false){
						list_ports[i].Init(port);
						break;
					}
				}
				obj2 = GameObject.Find(port);
			}
			if(GameObject.Find(head[0])){
				obj3 = GameObject.Find(head[0]);
			}else{
				obj3 = GameObject.Find("other");
			}
			srcPos = obj1.GetComponent<Renderer>().bounds.center;
			midPos = obj2.GetComponent<Renderer>().bounds.center;
			dstPos = obj3.GetComponent<Renderer>().bounds.center;
		}else{
			string[] data = fw_data.Split(',');
			if(GameObject.Find(data[0])){
				obj1 = obj3 = GameObject.Find(data[0]);
			}else{
				obj1 = obj3 = GameObject.Find("other");
			}
			if(data[1] == "ICMP"){//プロトコルを足して修正
				obj2 = GameObject.Find("ICMP");
			}else if(GameObject.Find(data[1])){
				obj2 = GameObject.Find(data[1]);
			}else{
				//ノットリッスンポート作成
				//事前にポートを作っておいて作成しておく
				for(int i = 0; i < list_ports.Count; i++){
					if(list_ports[i].gameObject.activeSelf == false){
						list_ports[i].Init(data[1]);
						break;
					}
				}
				obj2 = GameObject.Find(data[1]);
			}
			srcPos = obj1.GetComponent<Renderer>().bounds.center;
			midPos = obj2.GetComponent<Renderer>().bounds.center;
			dstPos = obj1.GetComponent<Renderer>().bounds.center;
			protocol = null;
		}
		//Vector3 srcPos = obj1.GetComponent<Renderer>().bounds.center;
		//Vector3 midPos = obj2.GetComponent<Renderer>().bounds.center;
		//Vector3 dstPos = obj3.GetComponent<Renderer>().bounds.center;
		for(int i = 0 ; i < list_Packets.Count; i++){
					if(list_Packets[i].gameObject.activeSelf == false){
						list_Packets[i].Init(srcPos, midPos, dstPos, protocol, type, obj2);
						return;
					}
		}
		return;
	}
	public void Packetlog(){
		string[] data = pcap_data.Split(',');
		string logdata = data[0] + ":" + data[2] + ">" + data[1] + ":" + data[3] + "." + data[4];
		Debug.Log(logdata);
	}
	public void Pcap(){
      	pcap_u.BeginReceive(new AsyncCallback(PcapCallback), pcap_s);
      	/*while (!pcap_Received){
    		Thread.Sleep(100);
  		}*/
      	//処理
    	Debug.Log(pcap_data);
    	taskFactory.StartNew(Packetlog);
    	taskFactory.StartNew(()=>{Movepacket(0);});
		return;
	}
	public void PcapCallback(IAsyncResult ar){
    	//UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
    	IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

    	Byte[] receiveBytes = pcap_u.EndReceive(ar, ref e);
    	pcap_data = Encoding.ASCII.GetString(receiveBytes);


    	pcap_Received = true;
    	Thread thread = new Thread(Pcap);
    	thread.Start();
	}


	public void Createaddress(){
		GameObject prefab = (GameObject)Resources.Load("src_ipaddress");
    	string[] data = add_data.Split(',');
    	if(createaddcount == 0){
	    	for(int i = 0; i < 9; i++){
				if(GameObject.Find(data[i]) == null && data[i] != "0.0.0.0"){
					Vector3 addressPosition;
					if(data[i] == hostadd){
						addressPosition = new Vector3(6,0,-20);
					}else{
						addressPosition = new Vector3(-10 + i * 5, 10 - i * 5, 20);
					}
					Quaternion q = new Quaternion();
					q = Quaternion.identity;
					GameObject obj = Instantiate(prefab, addressPosition, q) as GameObject;
					if(i < 8){
						obj.GetComponent<TextMesh> ().text = data[i];
						obj.name = data[i];
					}else{
						obj.GetComponent<TextMesh> ().text = obj.name = "other";
						obj.name = "other";
					}
				   	obj.SetActive(true);
				}
	      	}
	      	createaddcount++;
	    }else{
	    	for(int i = 0; i < 8; i++){
	    		if(data[i] != hostadd){
	    			Vector3 pos = GameObject.Find(data[i]).transform.position;
	    			pos.y = 10 - i*10;
	    		}
	    	}
	    }
		return;
	}
	public void Address(){
		add_u.BeginReceive(new AsyncCallback(AddressCallback), add_s);
    	taskFactory.StartNew(Createaddress);
		return;
	}
	public void AddressCallback(IAsyncResult ar){
    	//UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
    	IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

    	Byte[] receiveBytes = add_u.EndReceive(ar, ref e);
    	add_data = Encoding.ASCII.GetString(receiveBytes);


    	add_Received = true;
    	Thread thread = new Thread(Address);
    	thread.Start();
	}


	public void Createport(){
		GameObject prefab = (GameObject)Resources.Load("port");
    	string[] data = port_data.Split(',');
    	for(int i = 0; i < data.Length-1; i++){
			if(!GameObject.Find(data[i])){
				Vector3 portPosition = new Vector3(6 * (i % 4), 8 - (i / 4) * 3, 0);
				Quaternion q = new Quaternion();
				q = Quaternion.identity;
				prefab.GetComponent<TextMesh> ().text = data[i];
				GameObject obj = Instantiate(prefab, portPosition, q) as GameObject;
				obj.name = data[i];
				obj.SetActive(true);
			}
      	}
		return;
	}
	public void Port(){
		port_u.BeginReceive(new AsyncCallback(PortCallback), port_s);
		//処理
    	Debug.Log(port_data);
    	taskFactory.StartNew(Createport);
		return;
	}
	public void PortCallback(IAsyncResult ar){
    	//UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
    	IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

    	Byte[] receiveBytes = port_u.EndReceive(ar, ref e);
    	port_data = Encoding.ASCII.GetString(receiveBytes);


    	port_Received = true;
    	Thread thread = new Thread(Port);
    	thread.Start();
	}
	public void Firewall(){
		fw_u.BeginReceive(new AsyncCallback(FirewallCallback), fw_s);
		taskFactory.StartNew(()=>{Movepacket(1);});
		return;
	}
	public void FirewallCallback(IAsyncResult ar){
    	//UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
    	IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

    	Byte[] receiveBytes = fw_u.EndReceive(ar, ref e);
    	fw_data = Encoding.ASCII.GetString(receiveBytes);

    	fw_Received = true;
    	Thread thread = new Thread(Firewall);
    	thread.Start();
	}
	public void Rulemsg(){
		rule_u.BeginReceive(new AsyncCallback(RulemsgCallback), rule_s);
		Debug.Log(rule_data);
		return;
	}
	public void RulemsgCallback(IAsyncResult ar){
		//UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
    	IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

    	Byte[] receiveBytes = rule_u.EndReceive(ar, ref e);
    	rule_data = Encoding.ASCII.GetString(receiveBytes);

    	rule_Received = true;
    	Thread thread = new Thread(Rulemsg);
    	thread.Start();
	}
}