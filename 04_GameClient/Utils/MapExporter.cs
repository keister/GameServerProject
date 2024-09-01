// 2024081217:08
// 2024081217:08
// 김호준rkaghMy projectAssembly-CSharpMapExporter.cs


using System.Collections;
using System.Collections.Generic;
using System.IO;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.Serialization;

public class MapExporter : MonoBehaviour
{
    private static readonly byte obstacleTile = 0xFF;
    
    [SerializeField] private int width;
    [SerializeField] private int height;
    [Tooltip("1x1 크기를 해당 변수크기만큼으로 쪼갬")]
    [SerializeField] private int precisionWidth = 1;
    [SerializeField] private int precisionHeight = 1;
    [SerializeField] private int compressionWidth = 1;
    [SerializeField] private int compressionHeight = 1;
    [SerializeField] private LayerMask obstacle;
    [SerializeField] private LayerMask terrain;
    [SerializeField] private string filename;
    private byte[][] tile;
    private byte[][] sectionFilter;
    void Awake()
    {
        FileStream f = File.Create(filename);
        var writer = new BinaryWriter(f);
        FileStream f2 = File.Create("Text.txt");
        var writert = new StreamWriter(f2);

        int resolutionWidth = width * precisionWidth;
        int resolutionHeight = height * precisionHeight;
        
        writer.Write(resolutionWidth / compressionWidth);
        writer.Write(resolutionHeight / compressionHeight);
        writer.Write(precisionWidth / compressionWidth);
        writer.Write(precisionHeight / compressionWidth);
        
        Vector3 vec = new Vector3(0, 100f, 0);
        var ray = new Ray(vec, Vector3.down);
        tile = new byte[resolutionHeight][];
        sectionFilter = new byte[resolutionHeight][];
        for (int index = 0; index < resolutionHeight; index++)
        {
            tile[index] = new byte[resolutionWidth];
            sectionFilter[index] = new byte[resolutionWidth];
        }
        
        
        // export map data
        
        for (int z = 0; z < resolutionHeight; z++)
        {
            for (int x = 0; x < resolutionWidth; x++)
            {
                ray.origin = vec;
                RaycastHit hit;
                if (Physics.Raycast(ray, out hit, 200, obstacle | terrain))
                {
                    if ((1 << hit.collider.gameObject.layer) == obstacle)
                        tile[z][x] = 1;
                    else
                    {
                        tile[z][x] = 0;
                    }
                }
                else
                {
                    tile[z][x] = 0;
                }

                if (Physics.Raycast(ray, out hit, 200, LayerMask.GetMask("SectionFilter")))
                {
                    sectionFilter[z][x] = 1;
                }

                vec.x += (float)1 / precisionWidth;
            }

            vec.z += (float)1 / precisionHeight;
            vec.x = 0f;
        }


        // compression
        byte[][] compressedTile = new byte[resolutionHeight / compressionHeight][];
        byte[][] compressedSectionFilter = new byte[resolutionHeight / compressionHeight][];

        for (int i = 0; i < resolutionHeight / compressionHeight; i++)
        {
            compressedTile[i] = new byte[resolutionWidth / compressionWidth];
            compressedSectionFilter[i] = new byte[resolutionWidth / compressionWidth];
        }
        
        for (int z = 0; z < resolutionHeight; z+= compressionHeight)
        {
            for (int x = 0; x < resolutionWidth; x+= compressionWidth)
            {
                bool isObstacle = false;
                bool isFilter = false;
                for (int i = z; i < z + compressionHeight; i++)
                {
                    for (int j = x; j < x + compressionWidth; j++)
                    {
                        if (tile[i][j] == 1)
                        {
                            isObstacle = true;
                            i = z + compressionHeight;
                            break;
                        }
                    }
                }
                
                for (int i = z; i < z + compressionHeight; i++)
                {
                    for (int j = x; j < x + compressionWidth; j++)
                    {
                        if (sectionFilter[i][j] == 1)
                        {
                            isFilter = true;
                            i = z + compressionHeight;
                            break;
                        }
                    }
                }
                
                if (isObstacle)
                {
                    compressedTile[z / compressionHeight][x / compressionWidth] = 1;
                }
                else
                {
                    compressedTile[z / compressionHeight][x / compressionWidth] = 0;
                }
                
                if (isFilter)
                {
                    compressedSectionFilter[z / compressionHeight][x / compressionWidth] = 1;
                }
                else
                {
                    compressedSectionFilter[z / compressionHeight][x / compressionWidth] = 0;
                }
            }
        }

        
        
        GameObject go = GameObject.Find("@Section");
        int n = go.transform.childCount;

        SectionInfo[] infos = new SectionInfo[n];
        

        
        for (int i = 0; i < n; i++)
        {
            infos[i] = new SectionInfo();
            Transform child = go.transform.GetChild(i);

            infos[i].id = child.gameObject.GetComponent<SectionFilter>().filterId;
            FillFilter(compressedTile, compressedSectionFilter, (int)child.position.z, (int)child.position.x, infos[i].PosList);
            int cnt = child.childCount;
            for (int j = 1; j < cnt; j++)
            {
                Transform cchild = child.GetChild(j);
                MonsterFlag info = cchild.gameObject.GetComponent<MonsterFlag>();
                infos[i].MonsterSpawnInfos.Add(new MonsterSpawnInfo{id = info.id, 
                    x = cchild.position.x, 
                    y = cchild.position.z});
            }
        }


        foreach (var tileRow in compressedTile)
        {
            foreach (var tileN in tileRow)
            {
                writer.Write(tileN);
                writert.Write(tileN.ToString());
            }
            writert.Write("\n");
        }
        
        writer.Write(n);
        foreach (var info in infos)
        {
            writer.Write(info.id);
            writer.Write(info.PosList.Count);
            foreach (var pos in info.PosList)
            {
                writer.Write(pos.x + 0.5f);
                writer.Write(pos.y + 0.5f);
            }
            writer.Write(info.MonsterSpawnInfos.Count);
            foreach (var spawnInfo in info.MonsterSpawnInfos)
            {
                writer.Write(spawnInfo.id);
                writer.Write(spawnInfo.x);
                writer.Write(spawnInfo.y);
            }
        }
        
        
        StartCoroutine(Coroutinee());
        
        f.Close();
    }
    struct Pos
    {
        public int y;
        public int x;
    }

    struct MonsterSpawnInfo
    {
        public ulong id;
        public float x;
        public float y;
    }

    class SectionInfo
    {
        public int id;
        public List<Pos> PosList = new();
        public List<MonsterSpawnInfo> MonsterSpawnInfos = new();
    }
    void FillFilter(byte[][] comTile, byte[][] comFilter, int y, int x, List<Pos> list)
    {
        int[] dy = { 1, -1, 0, 0 };
        int[] dx = { 0, 0, 1, -1 };
        Queue<Pos> q = new();
        q.Enqueue(new Pos{y = y, x = x});

        while (q.Count != 0)
        {
            Pos now = q.Dequeue();
            
            for (int d = 0; d < 4; d++)
            {
                int nx = now.x + dx[d];
                int ny = now.y + dy[d];

                if (nx < 0 | nx >= comTile.Length | ny < 0 | ny >= comTile[0].Length)
                {
                    continue;
                }

                if (comTile[ny][nx] == 1)
                {
                    continue;
                }

                if (comTile[ny][nx] == 2)
                {
                    continue;
                }
                
                if (comFilter[ny][nx] == 1)
                {
                    continue;
                }

                Pos p = new Pos { y = ny, x = nx };
                comTile[ny][nx] = 2;
                q.Enqueue(p);
                list.Add(p);
            }
        }

        return;
    }

    IEnumerator Coroutinee()
    {
        while (true)
        {
            int resolutionWidth = width * precisionWidth;
            int resolutionHeight = height * precisionHeight;
        
            for (int z = 0; z < resolutionHeight; z+= compressionHeight)
            {
                for (int x = 0; x < resolutionWidth; x+= compressionWidth)
                {
                    bool isObstacle = false;
                
                    for (int i = z; i < z + compressionHeight; i++)
                    {
                        for (int j = x; j < x + compressionWidth; j++)
                        {
                            if (tile[i][j] == 1)
                            {
                                isObstacle = true;
                                i = z + compressionHeight;
                                break;
                            }
                        }
                    }

                    if (isObstacle)
                    {
                        for (int i = 0; i < compressionWidth * 10; i++)
                        {
                            float unit = (float)1 / precisionWidth / 10;
                            Debug.DrawLine(new Vector3((x / (float)precisionWidth) + unit * i, 20, (z / (float)precisionWidth)), 
                                new Vector3((x / (float)precisionWidth) + unit * i, 20, (z / (float)precisionWidth) + unit * 100), 
                                Color.blue);
                        }
                    }
                }
            }
        
            yield return null;
        }
        
    }
}