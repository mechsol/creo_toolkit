#include "./includes/ChangeDrwtoMdlName.h"

void ChangeDrwtoMdlName()
{
    ProError status;
    ProDrawing drawing;
    ProMdl tmpMdl;
    ProSolid solid;
    ProName mdlName, drawingName;
    ProPath drawingPath, currentPath;
    ProMdldata drawingData;
    int compResult;
    status = ProMdlCurrentGet(&drawing);
    status = ProMdlDataGet(drawing, &drawingData);
    status = ProDrawingCurrentsolidGet(drawing, &solid);
    status = ProDirectoryCurrentGet(currentPath);
    if (status == PRO_TK_NO_ERROR)
    {
        status = ProMdlNameGet(solid, mdlName);
        status = ProMdlNameGet(drawing, drawingName);
        status = ProWstringCompare(mdlName, drawingName, PRO_VALUE_UNUSED, &compResult);
        if (compResult != 0)
        {
            status = ProDirectoryChange(drawingPath);
            status = ProMdlRetrieve(mdlName, PRO_MDL_DRAWING, &tmpMdl);
            if (status != PRO_TK_NO_ERROR)
            {
                status = ProMdlRename(drawing, mdlName);
                status = ProMdlSave(drawing);
            }
            else
            {
                ShowMessageDialog(1, L"����δ��ɡ�\n��ͼ�ļ�Ŀ¼�´���ͬ���ļ������������");
            }
            status = ProDirectoryChange(currentPath);
        }
        else
        {
            ShowMessageDialog(1, L"����δ��ɡ�\n��ͼ�ļ���ģ��������ͬ��������ġ�");
        }
    }
}