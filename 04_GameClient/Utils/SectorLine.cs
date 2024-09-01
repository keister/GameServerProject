using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SectorLine : MonoBehaviour
{

    [SerializeField] private float width;
    [SerializeField] private float height;
    [SerializeField] private float sectorWidth;
    [SerializeField] private float sectorHeight;

    private Vector3 spoint;
    private Vector3 epoint;
    // Start is called before the first frame update
    void Start()
    {
        spoint = new Vector3(0f, 25f, 0f);
        epoint = new Vector3(width, 25f, height);

    }

    // Update is called once per frame
    void Update()
    {
        Vector3 eypoint = new Vector3(0, 25, height);
        Vector3 expoint = new Vector3(width, 25, 0);
        
        int sectorY = (int)height / (int)sectorHeight;
        int sectorX = (int)width / (int)sectorWidth;
        Vector3 xiter = new Vector3(sectorWidth, 0, 0);
        Vector3 yiter = new Vector3(0, 0, sectorHeight);
        for (int i = 0; i < sectorY + 1; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                Vector3 thick = new(0.03f, 0, 0);
                Debug.DrawLine(spoint + xiter * i + (thick * j) , eypoint + xiter * i + (thick * j), Color.red);
            }
        }
        
        for (int i = 0; i < sectorX + 1; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                Vector3 thick = new(0, 0, 0.03f);
                Debug.DrawLine(spoint + yiter * i + (thick * j), expoint + yiter * i + (thick * j), Color.red);
            }
            
        }
    }
}
