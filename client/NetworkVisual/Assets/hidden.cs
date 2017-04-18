using UnityEngine;
using System.Collections;

public class hidden : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		if(Input.GetMouseButtonUp(0)){
			this.gameObject.SetActive(false);	
		}
	}
}
