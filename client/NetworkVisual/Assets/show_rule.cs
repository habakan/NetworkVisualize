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

public class show_rule : MonoBehaviour {

	// Use this for initialization
	public GameObject rule_back;
	public GameObject rule_text;

	void Start () {
		//Debug.Log(rule_text.GetComponent<TextMesh>().text);
		//rule_back = rule_text = null;
		//rule_back = GameObject.Find("rule_back");
		//rule_text = GameObject.Find("rule_text");
		//rule_back.SetActive(false);
		//rule_text.SetActive(false);
	}
	// Update is called once per frame
	void Update () {
		if(Input.GetMouseButtonDown(0)){
			//Debug.Log("touch");	
			rule_back.SetActive(true);
			rule_text.SetActive(true);
		}
		if(Input.GetMouseButtonUp(0)){
			rule_back.SetActive(false);
			rule_text.SetActive(false);
		}
	}

}
