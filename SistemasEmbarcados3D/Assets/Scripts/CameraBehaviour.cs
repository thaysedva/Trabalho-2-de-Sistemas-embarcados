using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

namespace Assets.Scripts
{
    public class CameraBehaviour : MonoBehaviour
    {
        #region Properties

        public GameObject Target;

        public float OrbitSpeed = 2f;
        public float PanSpeed = .15f;
        public float ZoomSpeed = 5f;

        #endregion

        #region Fields

        private Vector3 defaultCameraPosition;

        private Camera cam;

        private bool canRotate;
        private bool canTranslate;

        #endregion

        #region Private Methods

        private void Start()
        {
            cam = GetComponent<Camera>();
            defaultCameraPosition = transform.position;
        }

        private void Update()
        {
            if (Input.GetMouseButtonDown(1) && !cam.orthographic && !IsPointerOverUIElement())
                canRotate = true;

            if (canRotate && Input.GetMouseButtonUp(1))
                canRotate = false;

            if (canRotate)
                UpdateRotation();

            if (Input.GetMouseButtonDown(2) && !IsPointerOverUIElement())
                canTranslate = true;

            if (canTranslate && Input.GetMouseButtonUp(2))
                canTranslate = false;

            if (canTranslate && !IsPointerOverUIElement())
                UpdatePosition();

            if (Math.Abs(Input.GetAxis("Mouse ScrollWheel")) > 0.001f && !IsPointerOverUIElement())
                UpdateZoom();
        }

        private void UpdateRotation()
        {
            transform.RotateAround(Target.transform.position, Vector3.up, Input.GetAxis("Mouse X") * OrbitSpeed);
            var pitchAngle = Vector3.Angle(Vector3.up, transform.forward);
            var pitchDelta = -Input.GetAxis("Mouse Y") * OrbitSpeed;
            var newAngle = Mathf.Clamp(pitchAngle + pitchDelta, 0f, 180f);
            pitchDelta = newAngle - pitchAngle;
            transform.RotateAround(Target.transform.position, transform.right, pitchDelta);
        }

        private void UpdatePosition()
        {
            var offset = transform.right * -Input.GetAxis("Mouse X") * PanSpeed +
                         transform.up * -Input.GetAxis("Mouse Y") * PanSpeed;

            transform.position += offset;
        }

        private void UpdateZoom()
        {
            var scrollInput = Input.GetAxis("Mouse ScrollWheel") * ZoomSpeed;
            if (cam.orthographic && cam.orthographicSize - scrollInput > 0)
                cam.orthographicSize -= scrollInput;

            if (!cam.orthographic || (transform.position + transform.forward * scrollInput).y > 1)
                transform.position += transform.forward * scrollInput;
        }

        #region UI

        public static bool IsPointerOverUIElement()
        {
            return IsPointerOverUIElement(GetEventSystemRaycastResults());
        }

        private static bool IsPointerOverUIElement(List<RaycastResult> eventSystemRaycastResults)
        {
            foreach (var curRaycastResult in eventSystemRaycastResults)
                if (curRaycastResult.gameObject.layer == LayerMask.NameToLayer("UI"))
                    return true;

            return false;
        }

        private static List<RaycastResult> GetEventSystemRaycastResults()
        {
            var eventData = new PointerEventData(EventSystem.current) { position = Input.mousePosition };
            var raycastResults = new List<RaycastResult>();
            EventSystem.current.RaycastAll(eventData, raycastResults);
            return raycastResults;
        }

        #endregion

        #endregion
    }
}