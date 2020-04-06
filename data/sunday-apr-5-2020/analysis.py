import pandas as pd
from matplotlib import pyplot as plt
from pandas.tseries import converter
converter.register()

dsco2 = pd.read_csv("ds-co2-20/s1.csv")
ts_dsco2 = pd.to_datetime(dsco2.iloc[:, 1])
values_dsco2 = dsco2.iloc[: ,0]
plt.plot(ts_dsco2, values_dsco2)
plt.show()