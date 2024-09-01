// 2024072422:07
// 2024072422:07
// 김호준rkaghMy projectAssembly-CSharpUI_HPBar.cs


using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UI_HPBar : UI_Base
    {
        enum GameObjects
        {
            HPBar
        }

        enum Texts
        {
            Name,
        }

        private Stat stat;
        public override void Init()
        {
            Bind<GameObject>(typeof(GameObjects));
            Bind<TMP_Text>(typeof(Texts));
            stat = transform.parent.GetComponent<Stat>();
        }

        void Awake()
        {
            Init();
        }

        private void Update()
        {
            Transform parent = transform.parent;  
            transform.position = parent.position + Vector3.up * (parent.GetComponent<Collider>().bounds.size.y);
            transform.rotation = Camera.main.transform.rotation;  // 빌보드
            float ratio =  (float)stat.Hp / (float)stat.MaxHp;
            SetHpRatio(ratio);  // 슬라이더 값 매 프레임마다 갱신
        }
        
        public void SetHpRatio(float ratio)
        {
            GetObject((int)GameObjects.HPBar).GetComponent<Slider>().value = ratio;
        }

        public void SetName(string name)
        {
            GetText((int)Texts.Name).text = name;
        }
    }
