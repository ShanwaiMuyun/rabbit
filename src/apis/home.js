import httpInstance from '@/utils/http'

// 获取 banner

export function getBannerAPI(params = {}) {
    // 默认为1，商品为2
    const { distributionSite = 1 } = params
    return httpInstance({
        url: '/home/banner',
        params: {
            distributionSite
        }
    })
}

// 获取新鲜好物

export function findNewAPI() {
    return httpInstance({
        url: '/home/new'
    })
}

// 获取人气推荐

export function getHotAPI() {
    return httpInstance({
        url: '/home/hot'
    })
}

// 获取产品列表

export function getGoodsAPI() {
    return httpInstance({
        url: '/home/goods'
    })
}