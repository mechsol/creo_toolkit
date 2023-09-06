#include "./includes/main.h"
#define QRCODE_MAX_LENGTH 128

int CurrentMdlType()
{
    ProMdl mdl;
    ProMdlType mdltype;
    ProError status;
    status = ProMdlCurrentGet(&mdl);
    if (status != PRO_TK_NO_ERROR)
        return -1;
    status = ProMdlTypeGet(mdl, &mdltype);
    if (status != PRO_TK_NO_ERROR)
        return -1;
    else
        return mdltype;
}

static uiCmdAccessState AccessDRW(uiCmdAccessMode access_mode)
{
    if (CurrentMdlType() == PRO_DRAWING)
        return ACCESS_AVAILABLE;
    else
        return ACCESS_INVISIBLE;
}

/// @brief ��ȡĬ���ָ�
/// @param mdl ��ǰ�Ļ�ͼ��ģ��(in)
/// @param height �ָߣ�out��
/// @return �����Ƿ�ɹ�
ProError ProUsrGetTextDefaultHeight(ProMdl mdl, double *height)
{
    ProError status;

    int cur_sheet;
    ProName w_size;
    ProMatrix matrix;

    status = ProSolidDefaulttextheightGet((ProSolid)mdl, height);

    if (status != PRO_TK_NO_ERROR)
        return status;

    status = ProDrawingCurrentSheetGet(mdl, &cur_sheet);
    status = ProDrawingSheetTrfGet(mdl, cur_sheet, w_size, matrix);
    if (status != PRO_TK_NO_ERROR)
        return status;

    *height /= matrix[0][0];
    return PRO_TK_NO_ERROR;
}

/// @brief �趨dtlnote��������ʽ
/// @param mdl ��ǰ��ͼ
/// @param text ��Ҫ�趨��dtlnote
/// @param height �ָ�
/// @param width �ֿ�
/// @param slant ��б�Ƕ�
/// @param thickness ���
/// @param wtext ����
/// @param
/// @return �����Ƿ�ɹ�
ProError ProUsrSetDtlText(ProMdl mdl, ProDtlnotetext *text, double height, double width, double slant, double thickness, wchar_t *wtext)
{
    ProError status;
    // �趨һ�е������
    status = ProUsrGetTextDefaultHeight(mdl, &height);
    status = ProDtlnotetextWidthSet(*text, width);
    status = ProDtlnotetextSlantSet(*text, slant);
    status = ProDtlnotetextThicknessSet(*text, thickness);
    status = ProDtlnotetextStringSet(*text, wtext);
    return status;
}

/// @brief �趨�����趨���õ�note
/// @param notedata dtlnote
/// @param attachment ���õķ�ʽ,�������ڴ�ǵ��ͷ�
/// @return �����Ƿ�ɹ�
ProError ProUsrSetFreeAttatchment(ProDtlnotedata *notedata, ProDtlattach *attachment)
{
    ProError status;
    ProMouseButton mouse_button;
    ProPoint3d pos;
    status = ProMousePickGet(PRO_LEFT_BUTTON, &mouse_button, pos);
    if (status != PRO_TK_NO_ERROR)
        return status;
    // �趨�ڷŷ�ʽ��������FREE����λ��֮ǰ�������λ��
    status = ProDtlattachAlloc(PRO_DTLATTACHTYPE_FREE, NULL, pos, NULL, attachment);
    status = ProDtlnotedataAttachmentSet(*notedata, *attachment);

    return status;
}

/// @brief �趨notedata����ɫ
/// @param notedata notedata
/// @return �����Ƿ�ɹ�
ProError ProUsrSetDefaultColor(ProDtlnotedata *notedata)
{
    ProError status;
    ProColor color;
    color.method = PRO_COLOR_METHOD_TYPE;
    color.value.type = PRO_COLOR_LETTER;
    status = ProDtlnotedataColorSet(*notedata, &color);
    return status;
}

ProError ProUsrNoteCreate()
{
    ProError status;
    ProMdl mdl;
    ProDtlnote note;
    ProDtlnotedata notedata;
    ProDtlnoteline line;
    ProDtlnotetext text;
    ProLine wtext;
    double height;
    ProColor color;
    ProDtlattach attachment;

    status = ProMdlCurrentGet(&mdl);
    if (status != PRO_TK_NO_ERROR)
        return status;

    status = ProMessageStringRead(PRO_LINE_SIZE, wtext);
    if (status != PRO_TK_NO_ERROR)
        return status;

    // һ��notedata�ж��line��ÿ��line��Ӧ���text���У�,ÿ����Ҫ�������ڴ棬�����������ͷ�
    status = ProDtlnotedataAlloc(mdl, &notedata);
    status = ProDtlnotelineAlloc(&line);
    status = ProDtlnotetextAlloc(&text);

    status = ProUsrSetFreeAttatchment(&notedata, &attachment);
    if (status == PRO_TK_NO_ERROR)
    {
        // �趨�����
        status = ProUsrGetTextDefaultHeight(mdl, &height);
        status = ProUsrSetDtlText(mdl, &text, height, 0.8, 0, 0, wtext);
        // line��һ�У����ԼӶ�δ������
        status = ProDtlnotelineTextAdd(line, text);
        // line����notedata
        status = ProDtlnotedataLineAdd(notedata, line);
        // �趨Ĭ����ɫ
        status = ProUsrSetDefaultColor(&notedata);
        // ��������ʾ
        status = ProDtlnoteCreate(mdl, NULL, notedata, &note);
        status = ProDtlnoteShow(&note);
    }

    // ������ڴ������ͷ�
    status = ProDtlnotedataFree(notedata);
    status = ProDtlnotetextFree(text);
    status = ProDtlnotelineFree(line);
    status = ProDtlattachFree(attachment);

    return status;
}

void SetInsertNote()
{
    ProError status;
    status = ProUsrNoteCreate();
}

int user_initialize()
{
    ProError status;
    uiCmdCmdId IMI_InsertNotemenuID;

    status = ProMenubarMenuAdd("IMI_NoteInsertmenu", "IMI_NoteInsertmenu", "About", PRO_B_TRUE, MSGFILE);

    status = ProCmdActionAdd("IMI_InsertNote_Act", (uiCmdCmdActFn)SetInsertNote, uiProeImmediate, AccessDRW, PRO_B_TRUE, PRO_B_TRUE, &IMI_InsertNotemenuID);
    status = ProMenubarmenuPushbuttonAdd("IMI_NoteInsertmenu", "IMI_InsertNotemenu", "IMI_InsertNotemenu", "IMI_InsertNotemenutips", NULL, PRO_B_TRUE, IMI_InsertNotemenuID, MSGFILE);

    return PRO_TK_NO_ERROR;
}

void user_terminate()
{
}
