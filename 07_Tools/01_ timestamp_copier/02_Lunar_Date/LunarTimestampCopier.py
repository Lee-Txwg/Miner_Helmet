# timestamp_copier.py
# 输出格式：
# [YYYY-MM-DD HH:MM:SS]  Author: Chan
# 农历X月X日  |  干支年 干支月 干支日

from datetime import datetime, date
import sys
try:
    from lunardate import LunarDate
except ImportError:
    print(f"⚠️ 未安装依赖: 使用 {sys.executable} -m pip install lunardate 安装 lunardate 模块")
    sys.exit(1)

# 天干地支列表
_STEMS = ["甲","乙","丙","丁","戊","己","庚","辛","壬","癸"]
_BRANCHES = ["子","丑","寅","卯","辰","巳","午","未","申","酉","戌","亥"]
_FIRST_MONTH_STEM = {
    # 年天干索引 : 当年正月的月天干索引
    0: 2, 1: 4, 2: 6, 3: 8, 4: 0,
    5: 2, 6: 4, 7: 6, 8: 8, 9: 0,
}


def _chinese_day(d: int) -> str:
    nums = ["","一","二","三","四","五","六","七","八","九"]
    if 1 <= d <= 10:
        return f"初{nums[d]}" if d != 10 else "初十"
    if 11 <= d <= 19:
        return f"十{nums[d-10]}"
    if d == 20:
        return "廿"
    if 21 <= d <= 29:
        return f"廿{nums[d-20]}"
    if d == 30:
        return "三十"
    return f"[{d}]"


def _julian_day(dt: date) -> int:
    y, m, d = dt.year, dt.month, dt.day
    if m < 3:
        y -= 1
        m += 12
    A = y // 100
    B = 2 - A + A // 4
    jd = int(365.25 * (y + 4716)) + int(30.6001 * (m + 1)) + d + B - 1524.5
    return int(jd + 0.5)


def _day_ganzhi(dt: date) -> str:
    # 使用通用偏移量49计算干支日
    jd = _julian_day(dt)
    idx = (jd + 49) % 60
    return _STEMS[idx % 10] + _BRANCHES[idx % 12]


def format_lunar(dt: datetime):
    # 格式化农历数字日期
    ld = LunarDate.fromSolarDate(dt.year, dt.month, dt.day)
    months_cn = ["","正","二","三","四","五","六","七","八","九","十","十一","腊"]
    lunar_num = f"农历{months_cn[ld.month]}月{_chinese_day(ld.day)}"

    # 干支年
    y_idx = (ld.year - 4) % 60
    year_gz = _STEMS[y_idx % 10] + _BRANCHES[y_idx % 12] + "年"

    # 干支月
    first_m_stem = _FIRST_MONTH_STEM[y_idx % 10]
    m_idx = (first_m_stem + ld.month - 1) % 10
    mb_idx = (ld.month + 1) % 12
    month_gz = _STEMS[m_idx] + _BRANCHES[mb_idx] + "月"

    # 干支日：一定要用公历日期 dt
    day_gz = _day_ganzhi(dt.date()) + "日"

    return lunar_num, f"{year_gz} {month_gz} {day_gz}"


def main():
    now = datetime.now()
    iso = now.strftime("%Y-%m-%d %H:%M:%S")
    header = f"[{iso}]  Author: Chan"
    try:
        lunar_num, gz = format_lunar(now)
        body = f"{lunar_num}  |  {gz}"
    except Exception as e:
        body = f"农历转换失败: {e}"
    output = f"{header}\n{body}"
    print(output)
    try:
        import pyperclip
        pyperclip.copy(output)
    except:
        pass

if __name__ == "__main__":
    main()