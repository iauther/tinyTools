from tencentcloud.common import credential
from tencentcloud.common.profile.client_profile import ClientProfile
from tencentcloud.common.profile.http_profile import HttpProfile
from tencentcloud.common.exception.tencent_cloud_sdk_exception import TencentCloudSDKException
from tencentcloud.ocr.v20181119 import ocr_client, models
import base64
import math
from PIL import Image
import os
import json
from pandas.core.frame import DataFrame
import pandas as pd

threshold = 2097152  # 文件缩小阈值
filePath = r'./'
output_filepath = filePath+r'\\压缩后文件'
data_excel = filePath + r'\\result.xlsx'
sid = ''
skey = ''


numlist = []
conlist = []
filenamelist = []
apilist = []
df = pd.DataFrame()
dfdetail = pd.DataFrame()



# 缩小图片
filelist = os.listdir(filePath)

os.mkdir(output_filepath)
for files in filelist:
    filename = filePath + '\\' + files
    output_filename = output_filepath + '\\' + files
    filesize = os.path.getsize(filename)
    if True:  # filesize >= threshold:
        print(filename)
        with Image.open(filename) as im:
            width, height = im.size
            if width >= height:
                new_width = int(math.sqrt(threshold / 2))
                new_height = int(new_width * height * 1.0 / width)
            else:
                new_height = int(math.sqrt(threshold / 2))
                new_width = int(new_height * width * 1.0 / height)
            resized_im = im.resize((new_width, new_height))
            # output_filename = filename.replace(source_dir, target_dir)
            resized_im.save(output_filename)

    # 图片base64
    with open(output_filename, "rb") as f:
        data = f.read()
        encodestr = base64.b64encode(data)  # 得到 byte 编码的数据
        # print(len(str(encodestr, 'utf-8')))
        picbase = str(encodestr, 'utf-8')

    dirc = {}

    try:
        cred = credential.Credential(sid, skey)
        httpProfile = HttpProfile()
        httpProfile.endpoint = "ocr.tencentcloudapi.com"

        clientProfile = ClientProfile()
        clientProfile.httpProfile = httpProfile
        client = ocr_client.OcrClient(cred, "ap-beijing", clientProfile)

        req = models.VatInvoiceOCRRequest()
        params = '{\"ImageBase64\":\"%s\"} ' % picbase
        req.from_json_string(params)

        resp = client.VatInvoiceOCR(req)
        reinfo = json.loads(resp.to_json_string())

        for i in reinfo["VatInvoiceInfos"]:
            count = 1
            name = i['Name']
            nameraw = i['Name']
            value = i['Value']
            if str(name) in dirc:
                while str(name) in dirc:
                    count = count + 1
                    name = nameraw + str(count)

                xx = {name: value}
                dirc.update(xx)
            else:

                xx = {name: value}
                dirc.update(xx)
        xx = {'路径': output_filename}
        dirc.update(xx)
        dfsingle = DataFrame(dirc, index=[0])
        df = df.append(dfsingle)
        #os.remove(output_filename)


    except TencentCloudSDKException as err:
        xx = {'路径': output_filename}
        dirc.update(xx)
        dfsingle = DataFrame(dirc, index=[0])
        df = df.append(dfsingle)
        print(err)

try:
    df1 = pd.DataFrame()
    df1 = df[['开票日期','发票号码', '货物或应税劳务、服务名称', '数量', '金额', '税额']]
    df1['行'] = '1'
    dfdetail = dfdetail.append(df1)
except:
    pass
try:
    df2 = pd.DataFrame()
    df2 = df[['开票日期','发票号码', '货物或应税劳务、服务名称2', '数量2', '金额2', '税额2']]
    df2.columns = ['开票日期','发票号码', '货物或应税劳务、服务名称', '数量', '金额', '税额']
    df2['行'] = '2'
    dfdetail = dfdetail.append(df2)
except:
    pass
try:
    df3 = pd.DataFrame()
    df3 = df[['开票日期','发票号码', '货物或应税劳务、服务名称3', '数量3', '金额3', '税额3']]
    df3.columns = ['开票日期','发票号码', '货物或应税劳务、服务名称', '数量', '金额', '税额']
    df3['行'] = '3'
    dfdetail = dfdetail.append(df3)
except:
    pass
try:
    df4 = pd.DataFrame()
    df4 = df[['开票日期','发票号码', '货物或应税劳务、服务名称4', '数量4', '金额4', '税额4']]
    df4.columns = ['开票日期','发票号码', '货物或应税劳务、服务名称', '数量', '金额', '税额']
    df4['行'] = '4'
    dfdetail = dfdetail.append(df4)
except:
    pass
try:
    df5 = pd.DataFrame()
    df5 = df[['发票号码', '货物或应税劳务、服务名称5', '数量5', '金额5', '税额5']]
    df5.columns = ['开票日期','发票号码', '货物或应税劳务、服务名称', '数量', '金额', '税额']
    df5['行'] = '5'
    dfdetail = dfdetail.append(df5)
except:
    pass
try:
    df6 = pd.DataFrame()
    df6 = df[['开票日期','发票号码', '货物或应税劳务、服务名称6', '数量6', '金额6', '税额6']]
    df6.columns = ['开票日期','发票号码', '货物或应税劳务、服务名称', '数量', '金额', '税额']
    df6['行'] = '6'
    dfdetail = dfdetail.append(df6)
except:
    pass
try:
    df7 = pd.DataFrame()
    df7 = df[['开票日期','发票号码', '货物或应税劳务、服务名称7', '数量7', '金额7', '税额7']]
    df7.columns = ['开票日期','发票号码', '货物或应税劳务、服务名称', '数量', '金额', '税额']
    df7['行'] = '7'
    dfdetail = dfdetail.append(df7)
except:
    pass
try:
    df8 = pd.DataFrame()
    df8 = df[['开票日期','发票号码', '货物或应税劳务、服务名称8', '数量8', '金额8', '税额8']]
    df8.columns = ['开票日期','发票号码', '货物或应税劳务、服务名称', '数量', '金额', '税额']
    df8['行'] = '8'
    dfdetail = dfdetail.append(df8)
except:
    pass
try:
    df9 = pd.DataFrame()
    df9 = df[['开票日期','发票号码', '货物或应税劳务、服务名称9', '数量9', '金额9', '税额9']]
    df9.columns = ['开票日期','发票号码', '货物或应税劳务、服务名称', '数量', '金额', '税额']
    df9['行'] = '9'
    dfdetail = dfdetail.append(df9)
except:
    pass
dfdetail['数量'] = pd.to_numeric(dfdetail['数量'])
dfdetail['金额'] = pd.to_numeric(dfdetail['金额'])
dfdetail['税额'] = pd.to_numeric(dfdetail['税额'])
dfdetail = dfdetail.sort_values(by=['发票号码','行'])
dfdetail = dfdetail.dropna(axis=0,thresh=4)
dfdetail['货物或应税劳务、服务名称'] = dfdetail['货物或应税劳务、服务名称'].fillna('无名称')
writer = pd.ExcelWriter(data_excel)
df.to_excel(writer, sheet_name='单行表', index=False)
dfdetail.to_excel(writer, sheet_name='详细表', index=False)


dfsum = dfdetail.groupby(['开票日期','发票号码', '货物或应税劳务、服务名称']).sum()

dfsum.to_excel(writer, sheet_name='统计表')

writer.save()
