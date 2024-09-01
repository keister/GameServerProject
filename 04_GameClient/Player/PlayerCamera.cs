using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace myproject
{
    public class PlayerCamera : MonoBehaviour
    {
        public Vector3 position = new Vector3(0, 60, -60);
        public Vector3 rotation = new Vector3(45, 0, 0);
        // Start is called before the first frame update
        void Start()
        {

        }

        // Update is called once per frame
        void Update()
        {
            transform.position = Player.Instance.character.transform.position + position;
            transform.localRotation = Quaternion.Euler(rotation);
        }
    }
}