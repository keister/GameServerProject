// 2024072421:07
// 2024072421:07
// 김호준rkaghMy projectAssembly-CSharpTargetView.cs


using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct CastInfo
{
    public bool Hit;                // ** 맞았는지 확인

    public Vector3 Point;           // ** 맞았다면 맞은 위치, 안맞았다면 Range 거리

    public float Distance;          // ** 도달 거리
    public float Angle;             // ** 각도
}
public class TargetView : MonoBehaviour
{
    [Header("Circle")]
    [Range(0, 30)]
    [SerializeField] private float viewRange = 15f;               // 시야 범위
    [Range(0, 360)]
    [SerializeField] private float viewAngle = 360f;               // 시야 각도

    [Header("Target")]
    [SerializeField] private LayerMask targetMask;          // 탐색 대상
    [SerializeField] private LayerMask obstacleMask;        // 장애물 대상
    [SerializeField] public GameObject target = null;
    [Header("Draw Line")]
    [Range(0.1f, 1f)]
    [SerializeField] private float angle = 1f;                   // 선이 표시될 각도. 작을 수록 선이 촘촘해진다.
    [SerializeField] private List<CastInfo> lineList;       // 표시된 선의 정보 리스트
    [SerializeField] private Vector3 offset;                // 위치 보정용 벡터. zero 로 해도 무관

    public void SetInfo(int layer, float range)
    {
        targetMask = layer;
        viewRange = range;
    }
    
    private void Start()
    {
        lineList = new();
        targetMask = LayerMask.GetMask("Monster");
        // 함수 실행 순서를 바꾸면 적 탐지 선이 가려져서 지워짐
        //StartCoroutine(DrawRayLine());
        StartCoroutine(CheckTarget());
    }

    IEnumerator CheckTarget()
    {
        WaitForSeconds wfs = new WaitForSeconds(0.1f);

        while (true)
        {
            target = null;
            float minDistance = 0f;
            GameObject curTarget = null;
            // =======================================================
            // 원형 범위 내 대상을 검출한다.
            Collider[] cols = Physics.OverlapSphere(transform.position, viewRange, targetMask);

            foreach (var e in cols)
            {
                // 검출한 대상의 방향을 구한다.
                Vector3 direction = (e.transform.position - transform.position).normalized;

                float distance = Vector3.Distance(e.transform.position, transform.position);

                if (curTarget == null)
                {
                    curTarget = e.gameObject;
                    minDistance = distance;
                }
                else
                {
                    if (minDistance > distance)
                    {
                        curTarget = e.gameObject;
                        minDistance = distance;
                    }
                }
                
                // 대상과의 각도가 설정한 각도 이내에 있는지 확인한다.
                // viewAngle 은 부채꼴 전체 각도이기 때문에, 0.5를 곱해준다.
                
            }

            target = curTarget;
            // =======================================================
            yield return null;
        }
    }

    IEnumerator DrawRayLine()
    {
        while (true)
        {
            lineList.Clear();       // 이미 생성된 레이캐스트 정보는 삭제한다.

            // 선이 표시될 갯수. 시야각에서 선이 표시될 각도를 나눠서 구한다.
            int count = Mathf.RoundToInt(viewAngle / angle) + 1;
            // 가장 오른쪽 각도. 시야각과 플레이어의 방향을 기준으로 결정된다.
            float fAngle = -(viewAngle * 0.5f) + transform.eulerAngles.y;

            // 선이 표시될 갯수만큼 실행한다.
            for (int i = 0; i < count; ++i)
            {
                // 해당 각도로 발사한 레이캐스트 정보를 가져온다.
                CastInfo info = GetCastInfo(fAngle + (angle * i));
                lineList.Add(info);

                // 해당 레이캐스트 정보에 따라 선을 그린다.
                Debug.DrawLine(transform.position + offset, info.Point, Color.green);
            }

            yield return null;
        }
    }


    private void OnDrawGizmos()
    {
        Gizmos.color = Color.green;
        //UnityEditor.Handles.DrawWireArc(transform.position, Vector3.up, Vector3.forward, 360f, viewRange);
    }

    CastInfo GetCastInfo(float _angle)
    {
    
        // 입력받은 각도에 따라 방향을 결정한다.
        Vector3 dir = new Vector3(Mathf.Sin(_angle * Mathf.Deg2Rad), 0.0f, Mathf.Cos(_angle * Mathf.Deg2Rad));
        CastInfo Info;
        RaycastHit hit;

        // 장애물에 맞는지 테스트
        if (Physics.Raycast(transform.position + offset, dir, out hit, viewRange, obstacleMask))
        {

            Info.Hit = true;                // 맞았는지 앉맞았는지 확인
            Info.Angle = _angle;            // 각도
            Info.Distance = hit.distance;   // 거리
            Info.Point = hit.point;         // 맞은 위치
        }

        // 장애물에 맞지 않았다면
        else
        {
            Info.Hit = false;               // 맞았는지 앉맞았는지 확인
            Info.Angle = _angle;            // 각도
            Info.Distance = viewRange;      // 맞지 않았다면 최대 도달 거리인 Range
            // 맞지 않았다면 해당 방향으로 최대 거리인 Range의 위치
            Info.Point = transform.position + offset + dir * viewRange;
        }

        return Info;
    }
}